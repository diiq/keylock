
Allows any modifier key to become a locking modifier; capslock is a locking version of shift. Keylock lets you make controllock, altlock, hyperlock, etc.

## Install

Run

    make; make install

By default, this will install in /usr/local/bin. If you'd like it somewhere else, then copy the executables to your `bin` directory of choice.

## Dependencies

- libxtst

## Usage

```keylock KEYCODE ...```

Where keycode is the key you'd like to lock. I, for instance, use

```keylock 105 108 &```

which sets my right alt and right control to be altlock and controllock.

Once you've tested and are happy with your locking, you can put the call in .xsession.

## Troubleshooting

While testing, you may discover that exiting keylock can leave you in a locked state (i.e., control is ALWAYS pressed). This is a bummer. If you whack the key in question a couple times -- to generate keyrelease events -- things usually come right.