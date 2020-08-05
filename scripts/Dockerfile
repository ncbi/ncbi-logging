FROM ubuntu:20.04
# docker build --build-arg PARSER_BRANCH=LOGMON-65 -t logmon_parser -f Dockerfile .
ARG PARSER_BRANCH=LOGMON-65
ARG SCRIPT_BRANCH=LOGMON-53

LABEL author="Mike Vartanian" \
      maintainer="mike.vartanian@nih.gov" \
      vendor="gov.nih.nlm.ncbi"

# prevent tzdata from asking where server is
RUN apt-get --quiet update && \
    DEBIAN_FRONTEND="noninteractive" \
    apt-get --quiet install -y make cmake git gcc g++ \
                               flex bison uuid-runtime sqlite3 awscli \
                               bison flex build-essential uuid-dev jq \
                               python3-pip

RUN pip3 -q install pre-commit influxdb gsutil gcloud

RUN git clone https://github.com/google/googletest && \
        cd googletest && \
        cmake .  && \
        make install

RUN git clone https://github.com/ncbi/ncbi-vdb3  && \
        cd ncbi-vdb3 && \
        git checkout engineering && \
        cd jwt-tool && \
        make

RUN git clone -b ${PARSER_BRANCH} --depth 1 https://github.com/ncbi/ncbi-logging && \
        cd /ncbi-logging/parser && \
        make && \
        ls -lah /ncbi-logging/parser/bin && \
        cp /ncbi-logging/parser/bin/* /

RUN cd /ncbi-logging/&& \
    git checkout -b ${SCRIPT_BRANCH} && \
    ls -lah

ENTRYPOINT /ncbi-logging/scripts/docker_parse.sh
