# RS3

Last **Java** client of each year. C++ / NXT builds are out of scope. The Java client was shut down on 18 December 2019.

| Year | Rev | Date | Gamepack |
|------|-----|------|----------|
| 2013 | 790 | 2013-12-16 | unpacked class jar |
| 2014 | 831 | 2014-12-04 | decrypted from in-jar parameters.txt and deobfuscated |
| 2015 | 860 | 2015-12-01 | decrypted from in-jar parameters.txt and deobfuscated |
| 2016 | 876 | 2016-11-28 | decrypted and unpacked (last pack in the mirror; no December build) |
| 2017 | 887 | 2017-11-01 | decrypted and unpacked (last pack in the mirror; no December build) |
| 2018 | 902 | 2018-12-14 | decrypted and unpacked |
| 2019 | 910 | 2019-10-16 | decrypted and unpacked (last 2019 pack; 912 is February 2020) |

OpenRS2's client catalog ends at RS2 **668** (2011). These jars come from the RuneWiki gamepack mirror on `archive.openrs2.org`. From 2014 the mirror stores an encrypted outer gamepack. `scripts/fetch_year_end_clients.py` decrypts it with the jav_config parameters that shipped inside that jar (`original/parameters.txt`, parameters `0` and `-1`) and unpacks pack200 into `original/gamepack.jar`. The encrypted download is kept as `original/gamepack-outer.jar`. 831 and 860 are decrypted that way and deobfuscated (`deob/gamepack-deob.jar` plus Gradle sources), same as the 2016–2019 packs. 790 was already an unpacked class jar and has no outer jar or parameters file.

RS3 disk caches for these revisions are several gigabytes each and are not downloaded. OpenRS2 did not have an exact cache id for 790, 860, or 887 in `work/caches.json`.

There is a Java *source refactor* of a December 2019 pack at [LostCityRS/RS910](https://github.com/LostCityRS/RS910). That tree is not the vanilla gamepack and is not what is archived here.
