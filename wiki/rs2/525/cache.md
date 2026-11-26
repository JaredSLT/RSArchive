# Cache

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

Kind is js5. Indexes are opened in `Client.java:1155-1182` by `SoftReferenceNodeFactory.opSoftReferenceNodeFactory16(discardPacked, discardUnpacked, bool3, index)`. The English archive name is not in those calls. Login sends 28 CRCs, indexes 0 through 27, including index 6 (`ObjStack.java:277-304`).

| Index | Field | Flags `(a, b, c)` |
| --- | --- | --- |
| 0 | `Cf46facc5.valuekind5548` | true, false, true |
| 1 | `Cf8f99191.valuekind5809` | true, false, true |
| 2 | `ObjStack.valueObjStack4` | false, true, true |
| 3 | `Ca157093a.valuekind36713` | true, false, true |
| 4 | `C747a18c6.valuekind5021` | true, false, true |
| 5 | `C05b1c6f1.valuekind5452` | true, true, true |
| 6 | `ChargementDesPolices.valuekind57612` | true, true, false |
| 7 | `Cee767227.valuekind5774` | true, false, true |
| 8 | `C09f9d4a12.valuekind4179` | true, false, true |
| 9 | `Client.valueClient1498` | true, false, true |
| 10 | `C09f9d4a12.valuekind4173` | true, false, true |
| 11 | `Cc0d52e40.valuekind5637` | true, false, true |
| 12 | `StartedLibrary.valuekind4849` | true, false, true |
| 13 | `TextureOp17.valueTextureOp1732` | true, false, true |
| 14 | `ObjStackNode.valueObjStackNode68` | true, false, false |
| 15 | `ChargementDesPolices.valuekind57610` | true, false, true |
| 16 | `ObjStackNode.valueObjStackNode79` | true, false, true |
| 17 | `TextureOp17.valueTextureOp1730` | true, false, true |
| 18 | `ChargementDesFichiersConfig.valuekind3996` | true, false, true |
| 19 | `RunescapeCom.valuekind4953` | true, false, true |
| 20 | `C3c7e15a6.valuekind5481` | true, false, true |
| 21 | `Ca0cef734.valuekind5561` | true, false, true |
| 22 | `ObjType.valueObjType195` | true, false, true |
| 23 | `C45440f3e.valuekind4801` | true, true, true |
| 24 | `JavaMouseWheel.valueJavaMouseWheel106` | true, false, true |
| 25 | `ByteArrayNodeSecondary.valueByteArrayNodeSecondary73` | true, false, true |
| 26 | `FichiersConfigChargBs.valuekind4691` | true, true, true |
| 27 | `C8e2aa65d.valuekind52911` | true, false, true |

## Loading stage 70

`Client.java:1231-1274` waits until indexes 2, 16, 17, 18, 19, 20, 21, 22, 24, 25, and 27 report at least 1100 groups together. Then:

| Call | Indexes |
| --- | --- |
| `C22edd1ce.opkind4092`, `Tile.call46b45dca`, `TextureOpVerticalGradient.opTextureOpVerticalGradient5`, `C6bf8523c.opkind5516`, `LinkedList.opLinkedList5`, `Ced8d3620.calld66e4f1e` | 2 |
| `ModuleTexteCharg.opVorbisMapping2` | 7 and 2 |
| `Cf05f8883.opkind4301` | 16 and 7 |
| `SoftReferenceNode.opSoftReferenceNode22` | 18 and 7 |
| `TitelbildGeOffnet.opByteArray20` | 7, `C98d1be1a.valuekind5214`, 19 |
| `SoftReferenceNodeFactory.opSoftReferenceNodeFactory17` | 20, 1, 0 |
| `WorldMapFont.opWorldMapFont17` | 21 and 7 |
| `Cc9ed28da.opByteArrayNodeSecondary77` | 22 |
| `ZugewiesenerSpeicher.opJs513` | 8, 7, 3, 13 |
| `SoftwareFont.opSoftwareFont83` | 17 |
| `BrokenOutputStream.opBrokenOutputStream9` and `MaterialManager_Class106.call6300ca88` | 24 and 25 |

Stage 45 (`Client.java:1188-1201`) starts audio from indexes 14, 15, 4, and 6. See [sound.md](sound.md). Map groups are read from index 5. See [region.md](region.md). The named interface `loginscreen` is looked up in index 3 (`Client.java:1379`). See [interfaces.md](interfaces.md).

`Light.call6c18fa80` (`Light.java:54-74`) resolves sprite names to group ids. The name strings live on other classes (`LightType.valueLightType24` and the rest of that argument list). Those string assignments were not opened, so the names are unread.

## Decode loops

Each loop reads `u8` opcodes until 0, then calls the method in the last column. Opcode bodies for these methods were not transcribed. See [gaps.md](gaps.md).

| Class | Loop | Dispatch |
| --- | --- | --- |
| `C8be0107e` | `call85e1b483` line 533 | `opkind36019` line 615. Built by `Cd07e1364.opkind4533` |
| `C3c656823` | `opkind5004` line 552 | `opkind50017` line 778. Built by `C2f21edda.opkind5731` |
| `FichiersConfigChargBs` | `opkind4697` line 326 | `opkind4693` |
| `MapElementList` | `opMapElementList24` line 161 | `opMapElementList25` |
| `C4ef674d8` | `opkind5228` line 433 | `opkind5223` |
| `C9e146a8e` | `call124d7712` line 474 | `opkind5083` |
| `C9c57aa59` | `callbd2283af` line 321 | `opkind3988` |
| `C90b6a93f` | `opkind4214` line 46 | `opkind4213` |
| `Ca3682c6a` | `opkind4564` line 147 | `opkind4561` |
| `C1b399fc7` | `opkind4593` line 130 | the body after the zero check |
| `C4832e8d5` | `opkind4074` line 175 | the body after the zero check |
| `C98d1be1a` | `opkind5216` line 835 | the body after the zero check |
| `Cc2a456cb` | `opkind5493` line 83 | the body after the zero check |
| `WallDecor` | `calldd94acee` line 140 | the body after the zero check |
| `WwwWtqa` | `opkind4345` line 116 | the body after the zero check |
| `TextureOp11` | `opTextureOp1111` line 89 | the body after the zero check |
| `FrameBuffer` | `opFrameBuffer8` line 27 | the body after the zero check |
| `Cfd4a3c76` | `opkind4145` line 49 | the body after the zero check |
| `ChargementDesPolices` | `opkind5767` line 149 | the body after the zero check |

Interface bytes are a separate pair of methods. See [interfaces.md](interfaces.md).

`NpcType.java` only holds `opNpcType27` and `opNpcType28`. It is not one of these loops.
