<sub>Reported by user @Ahtisilli at [Forum SailishOS Org (FSO)](https://forum.sailfishos.org/t/reviving-cepiperezs-flowplayer/17532/18?u=olf).</sub>

#### Xperia XA2 (32-bit)
Migrating from v0.1.x / v0.2.x version was a piece of cake: After starting and closing the new version (to see where the new file locations are), move/copy/symlink the configuration and database files from `~/.config/cepiperez/` to `~/.config/flowplayer/flowplayer/` and move the files in `~/.cache/flowplayer/` to `~/.cache/flowplayer/flowplayer`. For some reason the active configuration file is now in `cepiperez` and the active database in `flowplayer`.

#### Xperia 10 III (64-bit)
After a successful migration from 32-bit nemo to 64-bit defaultuser, I used `rsync -rtuv` over WiFi to copy FlowPlayer's files and the media-art cache (slow but elegant) and created symlinks so that the paths match - maybe I should look into SQLite3 to be able to fix the paths…
