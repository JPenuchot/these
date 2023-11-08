FROM archlinux/archlinux:base-devel

# Base Arch package installation

RUN --mount=type=cache,sharing=locked,target=/var/cache/pacman \
  pacman -Syu --noconfirm --needed \
  biber \
  texlive \
  ttf-opensans

# Run tests

WORKDIR /
COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
