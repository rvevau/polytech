#!/bin/bash

if [ "$EUID" -ne 0 ]; then
    echo "Требуются права root"
    exit 1
fi

LOG_FILE="/var/log/dns_installer.log"
CONFIG_BACKUP_DIR="/etc/dns_backup_$(date +%Y%m%d_%H%M%S)"
TMP_DIR="/tmp/dns_installer"

print_status() {
    echo "[INFO] $1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - INFO: $1" >> "$LOG_FILE"
}

print_warning() {
    echo "[WARN] $1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - WARN: $1" >> "$LOG_FILE"
}

print_error() {
    echo "[ERROR] $1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - ERROR: $1" >> "$LOG_FILE"
}

print_header() {
    echo "========================================"
    echo "$1"
    echo "========================================"
}

error_exit() {
    print_error "$1"
    exit 1
}

check_dependencies() {
    local deps=("wget" "curl" "grep" "awk")
    local missing_deps=()
    
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            missing_deps+=("$dep")
        fi
    done
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_status "Установка зависимостей: ${missing_deps[*]}"
        apt update && apt install -y "${missing_deps[@]}" || error_exit "Не удалось установить зависимости"
    fi
}

check_ports() {
    local port=53
    print_status "Проверка доступности порта $port..."
    
    if netstat -tuln | grep ":$port " > /dev/null; then
        print_warning "Порт $port занят"
        if command -v lsof > /dev/null; then
            local process=$(lsof -i :$port -sTCP:LISTEN | awk 'NR==2 {print $1}')
            print_warning "Порт $port используется процессом: $process"
        fi
        
        read -p "Продолжить установку? (это может привести к конфликту) (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 0
        fi
    fi
}

configure_network() {
    print_status "Проверка сетевых настроек..."
    
    if grep -q "127.0.0.1" /etc/resolv.conf; then
        print_status "localhost уже настроен как DNS-сервер"
    else
        print_status "Добавление localhost в resolv.conf..."
        cp /etc/resolv.conf /etc/resolv.conf.backup
        sed -i '1s/^/nameserver 127.0.0.1\n/' /etc/resolv.conf
    fi
    
    if [ -f /etc/sysctl.conf ]; then
        if ! grep -q "net.core.rmem_max" /etc/sysctl.conf; then
            print_status "Оптимизация сетевых параметров..."
            echo "net.core.rmem_max = 134217728" >> /etc/sysctl.conf
            echo "net.core.wmem_max = 134217728" >> /etc/sysctl.conf
            echo "net.core.somaxconn = 1024" >> /etc/sysctl.conf
            sysctl -p > /dev/null
        fi
    fi
}

create_backup() {
    print_status "Создание резервной копии текущей конфигурации..."
    mkdir -p "$CONFIG_BACKUP_DIR"
    
    if [ -d "/etc/bind" ]; then
        cp -r /etc/bind "$CONFIG_BACKUP_DIR/bind" 2>/dev/null
    fi
    
    if [ -d "/etc/unbound" ]; then
        cp -r /etc/unbound "$CONFIG_BACKUP_DIR/unbound" 2>/dev/null
    fi
    
    cp /etc/resolv.conf "$CONFIG_BACKUP_DIR/resolv.conf.backup" 2>/dev/null
}

choose_dns_server() {
    print_header "ВЫБОР DNS-СЕРВЕРА"
    echo "1. BIND9"
    echo "2. Unbound"
    echo "3. Выход"
    echo ""
    
    while true; do
        read -p "Выберите опцию (1-3): " choice
        case $choice in
            1)
                setup_bind
                break
                ;;
            2)
                setup_unbound
                break
                ;;
            3)
                echo "Выход..."
                exit 0
                ;;
            *)
                echo "Неверный выбор. Попробуйте снова."
                ;;
        esac
    done
}

setup_bind() {
    print_header "УСТАНОВКА BIND9"
    
    if dpkg -l | grep -q "bind9"; then
        print_warning "BIND9 уже установлен."
        read -p "Переустановить и перезаписать конфигурацию? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Установка отменена."
            return 1
        fi
        apt remove --purge -y bind9 bind9utils bind9-doc
    fi
    
    apt update >> "$LOG_FILE" 2>&1 || error_exit "Ошибка при обновлении пакетов"
    
    apt install -y bind9 bind9utils bind9-doc bind9-host >> "$LOG_FILE" 2>&1 || error_exit "Ошибка при установке BIND9"
    
    mkdir -p /etc/bind/zones
    
    cat > /etc/bind/named.conf.options << EOF
options {
    directory "/var/cache/bind";
    listen-on { any; };
    listen-on-v6 { any; };
    recursion yes;
    allow-recursion {
        localhost;
        127.0.0.1/32;
        192.168.0.0/16;
        10.0.0.0/8;
        172.16.0.0/12;
    };
    allow-transfer { none; };
    forwarders {
        1.1.1.1;
        8.8.8.8;
    };
    forward first;
    dnssec-validation auto;
    max-cache-size 256M;
    max-cache-ttl 3600;
    version "not disclosed";
    hide-version yes;
    querylog no;
    auth-nxdomain no;
    listen-on-v6 { any; };
};
EOF
    
    cat > /etc/bind/named.conf.logging << EOF
logging {
    channel default_log {
        file "/var/log/bind/bind.log" versions 3 size 5m;
        severity info;
        print-time yes;
        print-severity yes;
        print-category yes;
    };
    category default { default_log; };
    category queries { default_log; };
};
EOF
    
    echo 'include "/etc/bind/named.conf.logging";' >> /etc/bind/named.conf
    
    mkdir -p /var/log/bind
    chown bind:bind /var/log/bind
    chmod 755 /var/log/bind
    
    if named-checkconf; then
        print_status "Конфигурация корректна"
    else
        print_error "Ошибка в конфигурации"
        return 1
    fi
    
    chown -R bind:bind /etc/bind
    chmod -R 755 /etc/bind
    
    cat > /etc/systemd/system/bind9.service.d/override.conf << EOF
[Service]
ExecStartPre=/usr/sbin/named-checkconf -z /etc/bind/named.conf
ExecStart=/usr/sbin/named -f -u bind
EOF
    
    systemctl daemon-reload
    systemctl restart bind9
    systemctl enable bind9
    
    configure_firewall
    
    if systemctl is-active --quiet bind9; then
        print_status "BIND9 успешно запущен"
    else
        print_error "Ошибка запуска BIND9"
        journalctl -u bind9 -n 10 --no-pager
        return 1
    fi
    
    test_dns_server "bind9"
    
    print_status "Настройка BIND9 завершена!"
    echo "BIND9 успешно установлен и настроен"
    echo "Конфигурационные файлы: /etc/bind/"
    echo "Логи: /var/log/bind/bind.log"
    echo "Для проверки работы используйте: dig @localhost example.com"
}

setup_unbound() {
    print_header "УСТАНОВКА UNBOUND"
    
    if dpkg -l | grep -q "unbound"; then
        print_warning "Unbound уже установлен."
        read -p "Переустановить и перезаписать конфигурацию? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Установка отменена."
            return 1
        fi
        apt remove --purge -y unbound unbound-host
    fi
    
    apt update >> "$LOG_FILE" 2>&1 || error_exit "Ошибка при обновлении пакетов"
    
    apt install -y unbound unbound-host dnsutils >> "$LOG_FILE" 2>&1 || error_exit "Ошибка при установке Unbound"
    
    wget -O /var/cache/unbound/root.hints https://www.internic.net/domain/named.cache >> "$LOG_FILE" 2>&1
    
    unbound-anchor -a "/var/lib/unbound/root.key" >> "$LOG_FILE" 2>&1
    
    mkdir -p /var/log/unbound
    chown unbound:unbound /var/log/unbound
    
    cat > /etc/unbound/unbound.conf << EOF
server:
    interface: 0.0.0.0
    interface: ::0
    port: 53
    access-control: 127.0.0.0/8 allow
    access-control: 192.168.0.0/16 allow
    access-control: 10.0.0.0/8 allow
    access-control: 172.16.0.0/12 allow
    access-control: fc00::/7 allow
    access-control: ::1/128 allow
    hide-identity: yes
    hide-version: yes
    harden-glue: yes
    harden-dnssec-stripped: yes
    harden-referral-path: yes
    use-caps-for-id: no
    deny-any: yes
    val-clean-additional: yes
    num-threads: $(nproc)
    msg-cache-size: 64m
    rrset-cache-size: 128m
    key-cache-size: 32m
    cache-min-ttl: 300
    cache-max-ttl: 3600
    prefetch: yes
    prefetch-key: yes
    minimal-responses: yes
    auto-trust-anchor-file: "/var/lib/unbound/root.key"
    val-log-level: 2
    aggressive-nsec: yes
    val-nsec-type-flags: yes
    verbosity: 1
    logfile: "/var/log/unbound/unbound.log"
    log-time-ascii: yes
    log-queries: no
    log-replies: no
    log-local-actions: no
    root-hints: "/var/cache/unbound/root.hints"
    local-zone: "local." static
    local-zone: "10.in-addr.arpa." static
    local-zone: "16.172.in-addr.arpa." static
    local-zone: "17.172.in-addr.arpa." static
    local-zone: "18.172.in-addr.arpa." static
    local-zone: "19.172.in-addr.arpa." static
    local-zone: "20.172.in-addr.arpa." static
    local-zone: "21.172.in-addr.arpa." static
    local-zone: "22.172.in-addr.arpa." static
    local-zone: "23.172.in-addr.arpa." static
    local-zone: "24.172.in-addr.arpa." static
    local-zone: "25.172.in-addr.arpa." static
    local-zone: "26.172.in-addr.arpa." static
    local-zone: "27.172.in-addr.arpa." static
    local-zone: "28.172.in-addr.arpa." static
    local-zone: "29.172.in-addr.arpa." static
    local-zone: "30.172.in-addr.arpa." static
    local-zone: "31.172.in-addr.arpa." static
    local-zone: "168.192.in-addr.arpa." static
forward-zone:
    name: "."
    forward-addr: 1.1.1.1@853#cloudflare-dns.com
    forward-addr: 1.0.0.1@853#cloudflare-dns.com
    forward-addr: 8.8.8.8@853#dns.google
    forward-addr: 8.8.4.4@853#dns.google
    forward-tls-upstream: yes
EOF
    
    chown -R unbound:unbound /etc/unbound
    chmod -R 755 /etc/unbound
    
    if unbound-checkconf; then
        print_status "Конфигурация корректна"
    else
        print_error "Ошибка в конфигурации"
        return 1
    fi
    
    systemctl restart unbound
    systemctl enable unbound
    
    configure_firewall
    
    if systemctl is-active --quiet unbound; then
        print_status "Unbound успешно запущен"
    else
        print_error "Ошибка запуска Unbound"
        journalctl -u unbound -n 10 --no-pager
        return 1
    fi
    
    test_dns_server "unbound"
    
    print_status "Настройка Unbound завершена!"
    echo "Unbound успешно установлен и настроен"
    echo "Конфигурационный файл: /etc/unbound/unbound.conf"
    echo "Логи: /var/log/unbound/unbound.log"
    echo "Для проверки работы используйте: dig @localhost example.com"
}

configure_firewall() {
    if command -v ufw >/dev/null 2>&1; then
        if ufw status | grep -q "active"; then
            ufw allow 53/tcp >> "$LOG_FILE" 2>&1
            ufw allow 53/udp >> "$LOG_FILE" 2>&1
            print_status "Правила firewall для DNS добавлены"
        else
            print_status "UFW установлен, но не активен"
        fi
    else
        print_status "UFW не установлен, пропускаем настройку firewall"
    fi
    
    if command -v iptables >/dev/null 2>&1; then
        iptables -A INPUT -p udp --dport 53 -j ACCEPT >> "$LOG_FILE" 2>&1
        iptables -A INPUT -p tcp --dport 53 -j ACCEPT >> "$LOG_FILE" 2>&1
        print_status "Правила iptables для DNS добавлены"
    fi
}

test_dns_server() {
    local server_type=$1
    print_status "Тестирование DNS-сервера ($server_type)..."
    
    sleep 2
    
    if dig @127.0.0.1 google.com +short +time=2 +tries=1 > /dev/null 2>&1; then
        print_status "Базовое разрешение имен работает"
    else
        print_warning "Базовое разрешение имен не работает"
    fi
    
    if dig @127.0.0.1 -x 8.8.8.8 +short +time=2 +tries=1 > /dev/null 2>&1; then
        print_status "Обратное разрешение имен работает"
    else
        print_warning "Обратное разрешение имен не работает"
    fi
    
    if [ "$server_type" = "unbound" ]; then
        if dig @127.0.0.1 sigok.verteiltesysteme.net +dnssec > /dev/null 2>&1; then
            print_status "DNSSEC проверка работает"
        else
            print_warning "DNSSEC проверка не работает"
        fi
    fi
    
    local response_time=$(dig @127.0.0.1 google.com | grep "Query time" | awk '{print $4}')
    if [ -n "$response_time" ]; then
        print_status "Время отклика: ${response_time} мс"
    fi
}

install_utils() {
    apt install -y dnsutils bind9-dnsutils >> "$LOG_FILE" 2>&1 || print_warning "Не удалось установить дополнительные утилиты"
}

setup_monitoring() {
    cat > /usr/local/bin/check_dns_health << 'EOF'
#!/bin/bash
if dig @127.0.0.1 google.com +short +time=2 +tries=1 > /dev/null 2>&1; then
    echo "DNS сервер работает нормально"
    exit 0
else
    echo "Ошибка DNS сервера"
    exit 1
fi
EOF
    
    chmod +x /usr/local/bin/check_dns_health
    
    (crontab -l 2>/dev/null; echo "0 3 * * * /usr/local/bin/check_dns_health >> /var/log/dns_health.log 2>&1") | crontab -
    
    print_status "Мониторинг настроен"
}

main() {
    print_header "УСТАНОВКА DNS-СЕРВЕРА"
    echo "Этот скрипт установит и настроит DNS-сервер на вашей системе."
    echo "Все действия будут записаны в лог: $LOG_FILE"
    echo ""
    
    mkdir -p "$TMP_DIR"
    touch "$LOG_FILE"
    chmod 644 "$LOG_FILE"
    
    if ! grep -qi "debian\|ubuntu" /etc/os-release; then
        print_error "Этот скрипт поддерживает только Debian/Ubuntu системы."
        exit 1
    fi
    
    print_warning "ВНИМАНИЕ: Этот скрипт изменит сетевые настройки вашей системы."
    print_warning "Убедитесь, что вы понимаете последствия установки DNS-сервера."
    echo ""
    
    read -p "Продолжить установку? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Установка отменена."
        exit 0
    fi
    
    check_dependencies
    check_ports
    create_backup
    configure_network
    choose_dns_server
    install_utils
    setup_monitoring
    
    print_header "УСТАНОВКА ЗАВЕРШЕНА"
    echo "DNS-сервер успешно установлен"
    print_status "Резервная копия конфигурации создана в: $CONFIG_BACKUP_DIR"
    print_status "Подробный лог установки: $LOG_FILE"
}

trap "echo 'Скрипт прерван пользователем'; exit 1" SIGINT SIGTERM

main "$@"
