
# NOTES:


```
1000 microseconds == 1 millisecond
```
```
1000 milliseconds == 1 second
```
```
60 seconds == 1 minute
```


### formula to calculate the interval at which to send PPQ (pulse per quarter-note)
```
interval = (currentTime - timeOfLastRcordedClock) / PPQ
```

#### example:
```
interval = (1000000 - 500000) / 24
```

If the BPM is 120 then the average interval between quarter notes will be `500000` microseconds

If we want to send 24 pulses per quarter-note (PPQ) then the formula for the interval between pulses (in microseconds) would look like:
```
500000 / 24 = 20833.333333
```
