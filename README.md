Forked from http://sourceforge.net/projects/p7zip/

p7zip_explode (unix build)
==========================

Modified version of 7zip which supports archive exploding an archive into its constituent blocks. A block may 
contain more than one file. If the archive is setup in a certain way, each file can get its own block and then
exploding becomes more effective. 

## Building.
``` 
$ cd CPP/7zip/Bundles/Alone
$ make
```

## Usage:

Exploding:
```
$ ./7za p test/KISS_TEST_SET.7z -otest/

7-Zip (A) 9.20  Copyright (c) 1999-2010 Igor Pavlov  2010-11-18
Outputting into : test/

Exploding : test/KISS_TEST_SET.7z

Archive has 22 blocks
/ [0 blocks]
 KISS_TEST_SET [0 blocks]
  KISS_0000_Video_Baby [5 blocks]
  KISS_0000_Video_Parent [4 blocks]
  KISS_0000_Audio [3 blocks]
  KISS_0000_TEMP [3 blocks]
  KISS_0000_OXI [2 blocks]
   KISS_0307_BABY [3 blocks]
  KISS_0000_SURVEY [2 blocks]

Saving as 'test/KISS_TEST_SET/KISS_0000_Video_Baby/Baby_0000_1.AVI.7z'
Saving as 'test/KISS_TEST_SET/KISS_0000_Video_Baby/Baby_0000_2.AVI.7z'
Saving as 'test/KISS_TEST_SET/KISS_0000_Video_Baby/Baby_0000_3.AVI.7z'
Saving as 'test/KISS_TEST_SET/KISS_0000_Video_Baby/Baby_0000_4.AVI.7z'

```

The `-d` switch can be used to limit the explosion depth. For instance, if you only want to explode the archive into
a separate one for the first 2 directory levels, you'd do

` ./7za p archive.7z -d2 `

Finally, to checksum the data you use the `c` option:

```
$ ./7za c test/KISS_TEST_SET.7z

7-Zip (A) 9.20  Copyright (c) 1999-2010 Igor Pavlov  2010-11-18

Checking : test\KISS_TEST_SET.7z

D686A2E0
F4D9920D
1687120A
6AC8C777
C88AE9D7
FB022B1F
A03AC19E
C896DA50
41F8568B
475A5E7E
95AF17FC
D2C1772C
9763FB1F
F22A79A4
F7CE7DCF
117B562D
795D185C
860F41E2
6C4C0FAC
C2346AF7
93C62D49
175654DB
```

The checksums are printed starting from block 0.

## Tips:
If you are creating an archive and wish to explode it, you should ensure that each file is put into its own block. An 
easy way to achieve this is to create the archive with its compression level set to store. This, obviously, won't
compress the files but each file will get its own block. This shouldn't be an issure if you're packaging video files
which are already compressed anyway.
