<<<<<<< HEAD
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
=======
# MIA (Mon Intelligence Artificielle)
MIA is my take on basic multilayer perceptrons which utilizes the MNIST data set
to train a model on classifying hand written digits (0 - 9). It was written without
using any external libraries (apart from SDL for obvious reasons). Everything's from scratch*

## Features
- Minimalistic UI - I had no UI libraries so it was forced. :)
- Catalogue Mode where the neural network shows it's skills
- Draw Mode in which you can challenge MIA with your own drawing

## Stack
- C++ with SDL and my custom libraries
- Training and testing data and labels from the MNIST Database [https://github.com/cvdfoundation/mnist]

## Installation

Clone the repository:

```bash
git clone https://github.com/JoshuaDesmennu/ML.git
cd ML
```


Install dependencies:

```bash
npm install
```

## Running Locally

Start the server:

```bash
npm run start
```

Then open:

```txt
http://localhost:3000
```

## AI Usage

The only AI use was basic debugging and confirmation of stuff (When i forget how to use unordered_map)
Everything else (so basically everything) was done without AI.
>>>>>>> cd08d46 (added textbox for typing index, fixing initial click bug)
