# Sound

Status: gap.

The Java tree under `builds/rsc/115-2001-12-24/src/src/main/java` has no sound class, no synth, no vorbis, no midi, no wave, and no `AudioClip`. The packet switch in `Client.call7e894e6a` and `MudClient.call5fac7c41` has no sound opcode. There is no platform audio binding (`jagtheora` is absent). A later pass cannot add a sound path from this client. See `gaps.md`.
