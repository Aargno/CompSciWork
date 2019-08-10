from __future__ import print_function
import sys

def product(numbers) :
    return float(numbers[0]) * float(numbers[1])

numbers = sys.argv[1:]
print(product(numbers))