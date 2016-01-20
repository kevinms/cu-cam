![http://burnshudson.com/my_work/logo.jpg](http://burnshudson.com/my_work/logo.jpg)
```

Name:     Class Assignment Manager (CAM)
Language: C
Goals:    The intention is to greatly aid both the student and professor with
          the menial tasks that only annoy and consume time when done by hand.

Pleasent ASCII art to calm the nerves:

                +------------+
                |   (VM1)    |
        .------>| cam-server |<------.
        |       +------------+       |
        |              ^             |
        |              |             |
        |              |             |
        |       +------------+       |
        |   .-->| cam-admin  |<--.   |
        |   |   +------------+   |   |
        |   |                    |   |
        |   |                    |   |
        v   v                    v   v
   +------------+            +------------+
   |   (VM2)    |            |   (VM3)    |
   | cam-server |<---------->| cam-server |
   +------------+            +------------+
```