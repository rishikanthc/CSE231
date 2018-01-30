int foo(volatile unsigned a) {
  volatile unsigned x = 10;

  if (a > 5)
    return 4;
  else {
    if (a > 50) return foo(33);
    else a = 0;
  }

  return x+a;
}
