ARG BASE=ubuntu:20.04
ARG PROJECT_NAME=openll
ARG WORKSPACE=/workspace

# BUILD

FROM $BASE AS openll-build

ARG PROJECT_NAME
ARG WORKSPACE
ARG COMPILER_FLAGS="-j 4"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y --no-install-recommends sudo \
    && echo 'user ALL=(ALL) NOPASSWD: ALL' >/etc/sudoers.d/user
RUN apt install -y --no-install-recommends --reinstall ca-certificates
RUN apt install -y --no-install-recommends cmake git build-essential

ENV PROJECT_DIR="$WORKSPACE/$PROJECT_NAME"

WORKDIR $WORKSPACE

COPY --from=cpplocate:latest $WORKSPACE/cpplocate $WORKSPACE/cpplocate
COPY --from=cppassist:latest $WORKSPACE/cppassist $WORKSPACE/cppassist
COPY --from=cppfs:latest $WORKSPACE/cppfs $WORKSPACE/cppfs
COPY --from=globjects:latest $WORKSPACE/glm $WORKSPACE/glm
COPY --from=glbinding:latest $WORKSPACE/glfw $WORKSPACE/glfw
COPY --from=glbinding:latest $WORKSPACE/glbinding $WORKSPACE/glbinding
COPY --from=globjects:latest $WORKSPACE/globjects $WORKSPACE/globjects

RUN apt install -y --no-install-recommends \
    libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev
ENV glfw3_DIR=$WORKSPACE/glfw/lib/cmake

ENV glm_DIR=$WORKSPACE/glm/cmake

WORKDIR $WORKSPACE

ADD cmake $PROJECT_NAME/cmake
ADD docs $PROJECT_NAME/docs
ADD deploy $PROJECT_NAME/deploy
ADD source $PROJECT_NAME/source
ADD data $PROJECT_NAME/data
ADD CMakeLists.txt $PROJECT_NAME/CMakeLists.txt
ADD configure $PROJECT_NAME/configure
ADD $PROJECT_NAME-config.cmake $PROJECT_NAME/$PROJECT_NAME-config.cmake
ADD LICENSE $PROJECT_NAME/LICENSE
ADD README.md $PROJECT_NAME/README.md
ADD AUTHORS $PROJECT_NAME/AUTHORS

ENV CMAKE_PREFIX_PATH=$WORKSPACE

WORKDIR $PROJECT_DIR
RUN ./configure
RUN CMAKE_OPTIONS="-DOPTION_BUILD_TESTS=Off -DOPTION_BUILD_EXAMPLES=On -DCMAKE_INSTALL_PREFIX=$WORKSPACE/$PROJECT_NAME-install" ./configure
RUN cmake --build build -- $COMPILER_FLAGS
RUN cmake --build build --target install

# DEPLOY

FROM $BASE AS openll

ARG PROJECT_NAME
ARG WORKSPACE
ENV PROJECT_DIR="$WORKSPACE/$PROJECT_NAME"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y --no-install-recommends cmake libx11-6 nvidia-driver-470

WORKDIR $PROJECT_DIR

COPY --from=openll-build $WORKSPACE/cpplocate $WORKSPACE/cpplocate
COPY --from=openll-build $WORKSPACE/cppassist $WORKSPACE/cppassist
COPY --from=openll-build $WORKSPACE/cppfs $WORKSPACE/cppfs
COPY --from=openll-build $WORKSPACE/glm $WORKSPACE/glm
COPY --from=openll-build $WORKSPACE/glfw $WORKSPACE/glfw
COPY --from=openll-build $WORKSPACE/glbinding $WORKSPACE/glbinding
COPY --from=openll-build $WORKSPACE/globjects $WORKSPACE/globjects

COPY --from=openll-build $WORKSPACE/$PROJECT_NAME-install $WORKSPACE/$PROJECT_NAME

ENV LD_LIBRARY_PATH=$WORKSPACE/cpplocate/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/cppassist/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/cppfs/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/glfw/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/glbinding/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/globjects/lib
