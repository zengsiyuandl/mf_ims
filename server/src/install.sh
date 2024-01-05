#!/bin/bash

# Path variables
SRC_DIR=$(pwd)  # Save the current source directory
SERVICE_DIR="/etc/mf_imf/services"
APP_DATA_DIR="$SRC_DIR/app_data"
APACHE_CONF_DIR="/etc/apache2/conf-available"
MOD_HELLO_CONF="$SRC_DIR/adapter/apache/mod_hello.conf"
DAEMON_SCRIPT="$SERVICE_DIR/app_daemon.sh"
BUILD_RUN_SCRIPT="$SRC_DIR/build_and_run.sh"
COMPILE_APACHE_SCRIPT="$SRC_DIR/adapter/apache/compile_and_restart_apache.sh"
LOG_CONF_FILE="$SRC_DIR/log_app_data.conf"
DAEMON_FILE="$SRC_DIR/app_data/app_daemon.sh"
LOG_FOLDER="/var/log/app_data"

echo "Starting installation process..."

# Function to perform rollback
rollback() {
    echo "Error occurred. Performing rollback..."

    # Stop the daemon script
    pkill -f app_daemon

    # Stop any running instances of app_data
    pkill -f app_data

    # Check if mod_hello.conf was copied and remove it if yes
    if [ -f "$APACHE_CONF_DIR/mod_hello.conf" ]; then
        rm -f $APACHE_CONF_DIR/mod_hello.conf
        a2disconf mod_hello
    fi

    # Remove files from the target directory
    rm -rf "$SERVICE_DIR/*"

    # Remove build files
    rm -rf $APP_DATA_DIR/build

    echo "Rollback completed."
    exit 1
}

# Stop the daemon script
pkill -f app_daemon

# Stop any running instances of app_data
pkill -f app_data

# Remove existing files from the target directory
rm -rf "$SERVICE_DIR/*"

rm -rf "$LOG_FOLDER/*"

# Remove existing mod_hello.conf if it exists
if [ -f "$APACHE_CONF_DIR/mod_hello.conf" ]; then
    rm -f $APACHE_CONF_DIR/mod_hello.conf
    a2disconf mod_hello
fi

# Clear old compilation files in app_data directory
rm -rf $APP_DATA_DIR/build

# Run build_and_run.sh to compile and run app_data
$BUILD_RUN_SCRIPT || rollback
echo "app_data compiled and running."

# Copy log configuration file to target directory
mkdir -p "$SERVICE_DIR"
cp "$LOG_CONF_FILE" "$SERVICE_DIR" || rollback
echo "Log configuration file copied to $SERVICE_DIR."

cp "$DAEMON_FILE" "$SERVICE_DIR" || rollback
echo "Daemon file copied to $SERVICE_DIR."

# Copy mod_hello.conf to Apache's conf-available directory
cp $MOD_HELLO_CONF $APACHE_CONF_DIR || rollback

# Compile and restart Apache with the new module
$COMPILE_APACHE_SCRIPT || rollback
echo "Apache compiled and restarted with mod_hello."

chmod 755 $DAEMON_SCRIPT
# Start the daemon script in the background
nohup $DAEMON_SCRIPT > /dev/null 2>&1 &
echo "Daemon script started."

echo "Installation and setup completed successfully."
