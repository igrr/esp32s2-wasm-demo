FROM gitpod/workspace-full

ARG EMSDK_VER=2.0.8

RUN cd $HOME \
  && git clone -b ${EMSDK_VER} https://github.com/emscripten-core/emsdk.git \
  && cd emsdk \
  && ./emsdk install ${EMSDK_VER} \
  && ./emsdk activate ${EMSDK_VER} \
  && rm -rf zips

ENV EMSDK_PATH=$HOME/emsdk

COPY .gitpod.entrypoint.sh $HOME/entrypoint.sh

ENTRYPOINT [ "$HOME/entrypoint.sh" ]
CMD [ "/bin/bash" ]
