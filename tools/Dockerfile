FROM opensuse/leap

RUN zypper --non-interactive in\
    makedepend\
    make\
    wget\
    tar\
    bzip2\
    python2\
    python3\
    bash-completion\
    zsh\
    git\
#   libncurses5\
    libreadline6\
    libreadline7\
    libgtk-2_0-0\
    libgthread-2_0-0\
    libSM6\
    libGLU1

RUN useradd --no-log-init --create-home --shell /bin/zsh developer
WORKDIR /home/developer
RUN mkdir -p /home/developer/bin
RUN mkdir -p /home/developer/project
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
RUN tar -jxf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /home/developer/bin/
RUN rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
RUN echo "export PATH=$PATH:/home/developer/bin/gcc-arm-none-eabi-10.3-2021.10/bin" > /home/developer/.bashrc

RUN ZSH=/home/developer/.oh-my-zsh sh -c "$(wget -O- https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
RUN echo 'export ZSH="/home/developer/.oh-my-zsh"' > .zshrc
RUN echo 'ZSH_THEME="bira"' >> .zshrc
RUN echo 'plugins=(git)' >> .zshrc
RUN echo 'source $ZSH/oh-my-zsh.sh' >> .zshrc
RUN echo 'source $HOME/.bashrc' >> .zshrc

RUN chown -R developer:users /home/developer

CMD ["zsh", "-l"]
