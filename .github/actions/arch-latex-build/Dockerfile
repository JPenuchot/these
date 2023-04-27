FROM archlinux/archlinux:base-devel

# Base Arch package installation

RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm \
  biber \
  texlive-bibtexextra \
  texlive-fontsextra \
  texlive-most \
  ttf-opensans

# Run tests

WORKDIR /
COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
