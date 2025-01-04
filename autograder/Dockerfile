FROM gradescope/autograder-base:latest AS build1

RUN apt update \
    && apt install -y libssl-dev \
    && adduser student --no-create-home --disabled-password --gecos "" \
    && apt install -y tmux

FROM gradescope/autograder-base:latest AS build2
RUN apt update && apt install -y libssl-dev
COPY src /src
RUN cd /src && make libsocket.a && cp libsocket.a /usr/lib/ && make 

FROM build1
COPY --from=build2 /src/server /autograder/source/src/
COPY --from=build2 /src/client /autograder/source/src/
COPY --from=build2 /src/libsocket.a /usr/lib/
COPY autograder/run_autograder /autograder
COPY autograder /autograder/source
RUN pip install -r /autograder/source/requirements.txt
