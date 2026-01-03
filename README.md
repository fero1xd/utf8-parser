This is a really simple utf-8 parser written in C.

### Sample Input

```file=msg.txt
こaプレイ😄
```

### Sample Output

```
Read: 18 bytes
New line at: 17
New Buffer Length: 17
U+3053
U+61
U+30d7
U+30ec
U+30a4
U+1f604
こaプレイ😄
```
