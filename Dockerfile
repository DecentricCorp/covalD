FROM ubuntu:14.04
MAINTAINER Decentric <shannon@cov.al>
#reference https://bitcointa.lk/threads/linux-container-for-bitcoin.213739/
RUN mkdir /bitcoin-out && \
  apt-get -y update && \
  apt-get -y install python-software-properties && \
  apt-get -y install build-essential libtool autotools-dev autoconf pkg-config libssl-dev bsdmainutils git && \
  apt-get -y install libboost-all-dev && \
  apt-get -y install libminiupnpc-dev

RUN apt-get -y install -y libdb++-dev 
RUN apt-get install git -y 
WORKDIR ~/

RUN git clone https://github.com/synrg-labs/covald.git

RUN ./ribbitcoin/autogen.sh
RUN cd ribbitcoin && ./configure --with-incompatible-bdb --without-qt
RUN cd ribbitcoin && make

RUN apt-get build-dep wget -y
RUN apt-get install wget
RUN wget http://104.236.26.26:8080/chain.tar.gz

#RUN mkdir /root/ribbitcoin/
RUN tar -xvf chain.tar.gz -C /root/
COPY ribbitcoin.conf /root/ribbitcoin/ribbitcoin.conf
RUN mv /root/ribbitcoin/ /root/.ribbitcoin/

COPY run.sh run.sh
RUN cd ribbitcoin/src && cp ribbitcoind /usr/local/bin
RUN cd ribbitcoin/src && cp ribbitcoin-cli /usr/local/bin
RUN chmod u+x run.sh

EXPOSE 3763 3764

CMD ["./run.sh"]

#WORKDIR ~/ribbitcoin
