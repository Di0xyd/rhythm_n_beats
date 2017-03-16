# Projet IOT - Rythmn_hits

The goal of this project is to allow a drummer to train his rythmic accuracy.
It is composed of a metronome and a beat checker.

## Steps followed
We made the project by following these steps :

- Creating the metronome :
  - Code and hardware :
    - Light on each beat (On a fixed 60BPM beat)
    - Sound on each beat
    - Light on the first beat of a measure
      - IHM
        - get Current BPM
        - set new BPM

- Creating the beat checker
  - Code and hardware :
    - check for beats with vibration sensor
    - check if beats are good or bad
    - blink red led if bad
  - IHM :
    - show number of good beats
    - show number of wrong beats
    - Graphs of beats for 10 last measures

## Electrical schematic

     
