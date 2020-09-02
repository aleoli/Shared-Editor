#!/bin/bash
set -e

./scripts/build_client.sh
echo ""

sudo cp ./bin/client /bin/sharededitor

echo "Enter your default server IP:"
read -r ip

sudo mkdir -p "$HOME/.sharededitor/"
sudo cp ./src/client/gui/ui/resources/logo.png "$HOME/.sharededitor/"

desk=$(xdg-user-dir DESKTOP)
cat SharedEditor.desktop | sed "s/SERVER_IP/$ip/g" | sed "s~HOME~$HOME~g" > "$desk/SharedEditor.desktop"
chmod +x "$desk/SharedEditor.desktop"
