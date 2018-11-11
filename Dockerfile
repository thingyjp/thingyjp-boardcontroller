FROM debian:testing
RUN apt-get -qq update
RUN apt-get -qq install -y build-essential gcc-arm-none-eabi python
RUN useradd -m build
ADD . /home/build
RUN chown -R build:build /home/build
