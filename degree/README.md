# ESSY DEGREE++++




## Bit Fields

```
struct bits {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
};

struct bits mybits;
```

defines a 3-bit field (actually, it's three 1-bit felds). Bit operations now become a bit (haha) simpler:

To set or clear a bit:

```
mybits.b = 1;
mybits.c = 0;
```

To toggle a bit:

```
mybits.a = !mybits.a;
mybits.b = ~mybits.b;
mybits.c ^= 1;  /* all work */
```

Checking a bit:

```
if (mybits.c)  //if mybits.c is non zero the next line below will execute
```

This only works with fixed-size bit fields. Otherwise you have to resort to the bit-twiddling techniques described in previous posts.
