FROM archlinux/archlinux:base-devel

# Base Arch package installation

RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm texlab-most texlive-bibtexextra texlive-fontsextra

# Run tests

WORKDIR /
COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
