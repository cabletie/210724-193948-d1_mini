Autin's Creations code:
```
int iStart, iEnd;
iStart = 0;
iEnd = value.indexOf(',', iStart);
SoffitC = value.substring(iStart, iEnd).toInt();
iStart = iEnd + 1;
iEnd = value.indexOf(',', iStart);
SoffitR = value.substring(iStart, iEnd).toInt();
iStart = iEnd + 1;
iEnd = value.indexOf(',', iStart);
SoffitG = value.substring(iStart, iEnd).toInt();
iStart = iEnd + 1;
iEnd = value.indexOf(',', iStart);
SoffitB = value.substring(iStart, iEnd).toInt();
iStart = iEnd + 1;
iEnd = value.indexOf(',', iStart);
SoffitW = value.substring(iStart, iEnd).toInt();
```
