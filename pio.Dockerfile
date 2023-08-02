FROM gcc:latest

RUN apt-get update -y && apt-get upgrade -y

WORKDIR /usr/cmake/

RUN wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh

RUN  apt install -y build-essential libssl-dev libffi-dev python3-dev

RUN echo Y | bash cmake-3.26.4-linux-x86_64.sh 

ENV PATH="/usr/cmake/cmake-3.26.4-linux-x86_64/bin:$PATH"

RUN rm -rf cmake-3.26.4-linux-x86_64.sh

RUN apt-get install -y python3 python3-venv

RUN python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"

ENV PATH="/root/.platformio/penv/bin:$PATH"

WORKDIR /usr/workspace/