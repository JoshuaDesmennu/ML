# MIA - Mon Intelligence Artificielle

MIA is an AI (neural network) whose sole purpose in life is to guess the identity of handwritten digits.


## Technologies Used

MIA was made with C++ and [SDL2](https://www.libsdl.org/) for graphics. I wanted to do everything else from scratch so all matrix libraries
Neural network training, idx3 format parsers and everything else was written by hand. For my training and testing
data, I make use of the [MNIST database](https://github.com/cvdfoundation/mnist)


## How To Build

1. Clone the repository

```sh
git clone https://github.com/JoshuaDesmennu/ML.git
```


2. Install libsdl2

Ubuntu
```sh
sudo apt-get install libsdl2-dev
```


3. Run make

```sh
make
```


4. Run the executable

```sh
./mia
```

## AI Usage

I'm a strong believer in code it up yourself. But of course, I'm still human. The only AI usage was for error finding
and debugging, and also when i forget whether it's `.size()` or `.length()` :)
