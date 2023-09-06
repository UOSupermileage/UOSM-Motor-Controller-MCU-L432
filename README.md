# UOSM-Motor-Controller

[View Documentation](https://uosupermileage.github.io/UOSM-Motor-Controller-MCU-L432/#/)

## Getting Started
The team uses CLion for development. CLion is free for students. You will also need to install the compiler.

```bash
# MacOS
brew install --cask gcc-arm-embedded

# Ubuntu
# See https://lindevs.com/install-arm-gnu-toolchain-on-ubuntu

# Windows
# Install Ubuntu with WSL and then follow the Ubuntu instructions.
# Alternativly, Install the latest compiler from https://developer.arm.com/downloads/-/gnu-rm
# The WSL approach is heavily recommended.
```

```bash
# Make sure you've added your ssh key to GitHub.
# https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account

# Clone the project
git clone git@github.com:UOSupermileage/UOSM-Motor-Controller-MCU-L432.git

# Once you've made your changes
git commit -m "My changes"

git push origin main

# If you've modified UOSM-Core (Make sure you do this AFTER commiting your changes)
git subtree push --prefix UOSM-Core git@github.com:UOSupermileage/UOSM-Core.git main
```

## Contributing to Docs

Documentation is built with [Docsify](https://docsify.js.org/#/quickstart). This means there is no compiling! Editing the markdown files in /docs and commits to main will update the [Github Pages website](https://uosupermileage.github.io/UOSM-Motor-Controller-MCU-L432/#/) automatically!
