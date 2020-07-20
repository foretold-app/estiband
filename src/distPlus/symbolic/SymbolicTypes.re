type normal = {
  mean: float,
  stdev: float,
};

type lognormal = {
  mu: float,
  sigma: float,
};

type uniform = {
  low: float,
  high: float,
};

type beta = {
  alpha: float,
  beta: float,
};

type exponential = {rate: float};

type cauchy = {
  local: float,
  scale: float,
};

type pareto = {
  scale: float,
  shape: float
}

type triangular = {
  low: float,
  medium: float,
  high: float,
};

type symbolicDist = [
  | `Normal(normal)
  | `Beta(beta)
  | `Pareto(pareto)
  | `Lognormal(lognormal)
  | `Uniform(uniform)
  | `Exponential(exponential)
  | `Cauchy(cauchy)
  | `Triangular(triangular)
  | `Float(float) // Dirac delta at x. Practically useful only in the context of multimodals.
];

type analyticalSimplificationResult = [
  | `AnalyticalSolution(symbolicDist)
  | `Error(string)
  | `NoSolution
];
