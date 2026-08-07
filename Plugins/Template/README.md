# Template

## Type

- Base
- Desktop
- QtEvent
- Server

## Usage

### Script

[create_plugin.sh](../Script/create_plugin.sh) - Generate plugin from template

Usage: ./Script/create_plugin.sh [OPTION] PluginName

Options:
  -h, --help            Show this help message
  -v, --verbose[=LEVEL] Set verbose mode. [LEVEL: ON, OFF]

Directory options:
  -i, --install=DIR         Set installation directory

Other options:
  -n, --name=NAME           Plugin name
  -t, --template=NAME       Template name. [NAME: Base(Default), Desktop, Server, QtEvent]

Examples:
  ./Script/create_plugin.sh --name=Test --template=Base
  ./Script/create_plugin.sh -n Test -t Base