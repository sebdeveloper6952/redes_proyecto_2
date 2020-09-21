FROM ubuntu:bionic

WORKDIR /home/app

COPY . .

RUN apt update && apt install -y \
    build-essential \
    pkg-config \
    libssl-dev \
    libexpat-dev \
    libstrophe-dev \
    libncurses5-dev \
    libncursesw5-dev

RUN make

CMD [ "./ncurses_client" ]