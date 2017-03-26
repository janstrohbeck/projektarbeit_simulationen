FROM ubuntu:zesty
MAINTAINER Jan Strohbeck <jan@strohbeck.net>
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -q && apt-get install -qy \
    build-essential texlive-full \
    python-pygments gnuplot \
    python-sphinx pandoc \
    make python3 openjdk-8-jre \
    git python python-setuptools wget \
    unzip graphviz

RUN git clone https://github.com/aclements/latexrun.git && cp latexrun/latexrun /usr/bin/ && chmod +x /usr/bin/latexrun && rm -r latexrun
RUN wget -O plantuml.jar http://sourceforge.net/projects/plantuml/files/plantuml.jar/download && mkdir -p /opt/plantuml && mv plantuml.jar /opt/plantuml/plantuml.jar
RUN wget -O adadomain.zip https://www.jan-strohbeck.com/adadomain.zip && unzip adadomain.zip && cd adadomain && python setup.py build && python setup.py install

RUN apt-get autoclean
RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

WORKDIR /data
VOLUME ["/data"]
