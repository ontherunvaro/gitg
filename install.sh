#!/usr/bin/env bash
flatpak-builder build-dir org.gnome.Gitg.json --force-clean --install --install-deps-from=flathub
