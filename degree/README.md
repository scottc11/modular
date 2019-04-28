# DEGREE++++

Short Demo --> [https://www.instagram.com/p/BtwLhoBhjfV/?utm_source=ig_web_copy_link](https://www.instagram.com/p/BtwLhoBhjfV/?utm_source=ig_web_copy_link)

Long Demo --> [https://www.youtube.com/watch?v=UvROb2ZHefI](https://www.youtube.com/watch?v=UvROb2ZHefI)

---

### What is it?
For lack of a better term --> **Four Channel Voltage Generator / Quantizer**

### What does it do?
Outputs 4 seperate channels of quantized voltages based on the [1 volt per octave](https://en.wikipedia.org/wiki/CV/gate) protocol

### WHY?

Ever since my modular system grew from having one oscillator to two oscillators, I have been plagued with the problem of keeping the oscillators in harmony with each other. This problem led me to create an interface for **controlling/managing/PERFORMING harmonies** between two or more oscillators. I suppose if money was no issue one could buy four individual "quantizer" modules - but even a standard quantizer doesn't support the ability to output a single quantized 1 volt per octave value to an oscillator without supplying a voltage as an input. What if I just want to output a specific note, regardless of an external (and somewhat random) voltage source? I suppose there are MIDI-to-CV modules which I could hook up to a MIDI keyboard, but then if I wanted to control 4 voices at the same time I would then have to wire up FOUR MIDI keyboards 😐. Yikes.


# How It Works:

There are **four channels**, and each channel operates independently.

Each channel has the following hardware:
- CV input
- GATE output
- 1V/octave quantized CV output
- 8 touch sensitive buttons with LEDs indicating which scale degrees are active / inactive
- Pornomento / glide analog circuit via slide pots
- octave up/down switch (4 octaves total)
- "mode" switch for toggling between what I call `monophonic` mode and `quantizer` mode


#### Scale Degrees
A series of 8 toggle switches determines the active scale that gets applied to all four channels.  The first switch represents the root of the scale, the second switch represents the second degree of the scale, the third switch represents the third degree of the scale, etc.

Depending on the state of each toggle switch, the corrosponding scale degree is either `major` or `minor`

```
toggle switch -------->  VIII  -----> major/minor
toggle switch -------->  VII   -----> major/minor
toggle switch -------->  VI    -----> major/minor
toggle switch -------->  V     -----> major/minor
toggle switch -------->  IV    -----> major/minor
toggle switch -------->  III   -----> major/minor
toggle switch -------->  II    -----> major/minor
toggle switch -------->  I     -----> major/minor
```

This lets you build a scale that is either **MAJOR, MINOR, AUGMENTED, or DIMINISHED**.  The next version I make will likely alter the logic of these switches. I am considering changing them all to three stage toggle switches to give more interesting scale options.

#### The MODES:
There are two modes available via each channels "mode" toggle switch. **monophonic** mode and **quantizer** mode.

##### Monophonic:
Pays no attention to any incoming CV, and simply outputs which ever scale degree is currently active/selected (via touch buttons, indicated by an LED).

##### Quantizer:
Takes an incoming voltage source and quantizes it to a 1 volter per octave value (determined by the scale selection toggle switches). You can activate/deactivate which scale degrees you want the incoming voltage to be mapped to by touching the channels buttons.



## Hardware:
I will expand on this list eventually, but for the time being these are the basic ICs I used for the project.  
- Atmega328 microcontoller
- MPR121 Capacitive Touch IC
- MCP23017 IO expander
- MCP4922 DAC
