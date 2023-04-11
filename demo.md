You can launch the Poutine shell by running

    make run

As of this writing, Poutine isn't much of a programming language; it's merely a program that lets you store and retrieve data in a rather tedious way. Most notably, although you can create lists, there is not yet any way to _print_ a list! I will probably implement that feature next, however.

In order to use Poutine, we must first create an atom. If you type `atom nil`, then Poutine will print `0`:

    > atom nil
    0

We just asked Poutine to create an atom called `nil`. It did that, and it placed our new atom in location 0 on the heap. Now we can ask Poutine what the atom in location 0 is:

    > getatom 0
    nil

Next, let's create a list! We'll create the list `(one two)`. If you've guessed that Poutine is not yet able to parse lists, then sadly, you're correct. In order to create this list, we have to manually create both of the cons cells that make it up.

Let's start by creating the atoms `one` and `two`:

    > atom one
    1
    > atom two
    2

Next, we can create the list `(two)` by forming a cons cell whose car is `two` and whose cdr is `nil`. Remember that the atom `two` is in location 2 and the atom `nil` is in location 0. (I told you this would be tedious!) We can run:

    > cons 2 0
    3

Now, location 3 contains a cons cell which represents the list `(two)`.

How can we confirm that that's what's in location 3? The `gettag` command tells us what type of value is in a cell:

    > gettag 3
    cons

Poutine just told us that location 3 contains a cons cell. We can find out what the car and cdr of the cons cell are by using the `getcar` and `getcdr` commands:

    > getcar 3
    2
    > getcdr 3
    0

In case you've forgotten what's in locations 2 and 0, we can use the `gettag` command to find out what types of values they contain, and if it turns out that they contain atoms, we can use `getatom` to get their text.

    > gettag 2
    atom
    > getatom 2
    two
    > gettag 0
    atom
    > getatom 0
    nil

Finally, since location 3 contains the list `(two)` and location 1 contains the atom `one`, we can create the list `(one two)` like so:

    > cons 1 3
    4

If, for some reason, you're not bored yet, you can now use the `gettag` and `getatom` commands to verify that location 4 does, in fact, contain a cons cell representing the list `(one two)`.

You can exit the shell by pressing either Ctrl-C (interrupt) or Ctrl-D (end of file).
