FROM ubuntu:22.04
USER root
RUN apt-get update
RUN apt-get install -y cmake clang g++ git libpthread-stubs0-dev python3 python3-pip wget
RUN pip3 install numpy matplotlib networkx pandas bokeh
CMD ["tail", "-f", "/dev/null"]