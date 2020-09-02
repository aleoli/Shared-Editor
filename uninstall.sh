#!/bin/bash
set -e

sudo rm /bin/sharededitor
sudo rm -rf "$HOME/.sharededitor/"

desk=$(xdg-user-dir DESKTOP)
sudo rm "$desk/SharedEditor.desktop"
