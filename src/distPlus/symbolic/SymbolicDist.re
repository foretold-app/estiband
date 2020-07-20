open SymbolicTypes;

module Exponential = {
  type t = exponential;
  let pdf = (x, t: t) => Jstat.exponential##pdf(x, t.rate);
  let cdf = (x, t: t) => Jstat.exponential##cdf(x, t.rate);
  let inv = (p, t: t) => Jstat.exponential##inv(p, t.rate);
  let sample = (t: t) => Jstat.exponential##sample(t.rate);
  let mean = (t: t) => Ok(Jstat.exponential##mean(t.rate));
  let toString = ({rate}: t) => {j|Exponential($rate)|j};
};

module Cauchy = {
  type t = cauchy;
  let pdf = (x, t: t) => Jstat.cauchy##pdf(x, t.local, t.scale);
  let cdf = (x, t: t) => Jstat.cauchy##cdf(x, t.local, t.scale);
  let inv = (p, t: t) => Jstat.cauchy##inv(p, t.local, t.scale);
  let sample = (t: t) => Jstat.cauchy##sample(t.local, t.scale);
  let mean = (_: t) => Error("Cauchy distributions have no mean value.");
  let toString = ({local, scale}: t) => {j|Cauchy($local, $scale)|j};
};

module Triangular = {
  type t = triangular;
  let pdf = (x, t: t) => Jstat.triangular##pdf(x, t.low, t.high, t.medium);
  let cdf = (x, t: t) => Jstat.triangular##cdf(x, t.low, t.high, t.medium);
  let inv = (p, t: t) => Jstat.triangular##inv(p, t.low, t.high, t.medium);
  let sample = (t: t) => Jstat.triangular##sample(t.low, t.high, t.medium);
  let mean = (t: t) => Ok(Jstat.triangular##mean(t.low, t.high, t.medium));
  let toString = ({low, medium, high}: t) => {j|Triangular($low, $medium, $high)|j};
};

module Normal = {
  type t = normal;
  let pdf = (x, t: t) => Jstat.normal##pdf(x, t.mean, t.stdev);
  let cdf = (x, t: t) => Jstat.normal##cdf(x, t.mean, t.stdev);

  let from90PercentCI = (low, high) => {
    let mean = E.A.Floats.mean([|low, high|]);
    let stdev = (high -. low) /. (2. *. 1.644854);
    `Normal({mean, stdev});
  };
  let inv = (p, t: t) => Jstat.normal##inv(p, t.mean, t.stdev);
  let sample = (t: t) => Jstat.normal##sample(t.mean, t.stdev);
  let mean = (t: t) => Ok(Jstat.normal##mean(t.mean, t.stdev));
  let toString = ({mean, stdev}: t) => {j|Normal($mean,$stdev)|j};

  let add = (n1: t, n2: t) => {
    let mean = n1.mean +. n2.mean;
    let stdev = sqrt(n1.stdev ** 2. +. n2.stdev ** 2.);
    `Normal({mean, stdev});
  };
  let subtract = (n1: t, n2: t) => {
    let mean = n1.mean -. n2.mean;
    let stdev = sqrt(n1.stdev ** 2. +. n2.stdev ** 2.);
    `Normal({mean, stdev});
  };

  // TODO: is this useful here at all? would need the integral as well ...
  let pointwiseProduct = (n1: t, n2: t) => {
    let mean =
      (n1.mean *. n2.stdev ** 2. +. n2.mean *. n1.stdev ** 2.)
      /. (n1.stdev ** 2. +. n2.stdev ** 2.);
    let stdev = 1. /. (1. /. n1.stdev ** 2. +. 1. /. n2.stdev ** 2.);
    `Normal({mean, stdev});
  };

  let operate = (operation: Operation.Algebraic.t, n1: t, n2: t) =>
    switch (operation) {
    | `Add => Some(add(n1, n2))
    | `Subtract => Some(subtract(n1, n2))
    | _ => None
    };
};

module Beta = {
  type t = beta;
  let pdf = (x, t: t) => Jstat.beta##pdf(x, t.alpha, t.beta);
  let cdf = (x, t: t) => Jstat.beta##cdf(x, t.alpha, t.beta);
  let inv = (p, t: t) => Jstat.beta##inv(p, t.alpha, t.beta);
  let sample = (t: t) => Jstat.beta##sample(t.alpha, t.beta);
  let mean = (t: t) => Ok(Jstat.beta##mean(t.alpha, t.beta));
  let toString = ({alpha, beta}: t) => {j|Beta($alpha,$beta)|j};
};

module Pareto = {
  type t = pareto;
  let pdf = (x, t: t) => Jstat.pareto##pdf(x, t.scale, t.shape);
  let cdf = (x, t: t) => Jstat.pareto##cdf(x, t.scale, t.shape);
  let inv = (p, t: t) => Jstat.pareto##inv(p, t.scale, t.shape);
  let mean = (t: t) => Ok(Jstat.pareto##mean(t.scale, t.shape));
  let sample = (t: t) => inv(Random.float(1.), t);
  let toString = ({scale, shape}: t) => {j|Beta($scale,$shape)|j};
};

module Lognormal = {
  type t = lognormal;
  let pdf = (x, t: t) => Jstat.lognormal##pdf(x, t.mu, t.sigma);
  let cdf = (x, t: t) => Jstat.lognormal##cdf(x, t.mu, t.sigma);
  let inv = (p, t: t) => Jstat.lognormal##inv(p, t.mu, t.sigma);
  let mean = (t: t) => Ok(Jstat.lognormal##mean(t.mu, t.sigma));
  let sample = (t: t) => Jstat.lognormal##sample(t.mu, t.sigma);
  let toString = ({mu, sigma}: t) => {j|Lognormal($mu,$sigma)|j};
  let from90PercentCI = (low, high) => {
    let logLow = Js.Math.log(low);
    let logHigh = Js.Math.log(high);
    let mu = E.A.Floats.mean([|logLow, logHigh|]);
    let sigma = (logHigh -. logLow) /. (2.0 *. 1.645);
    `Lognormal({mu, sigma});
  };
  let fromMeanAndStdev = (mean, stdev) => {
    let variance = Js.Math.pow_float(~base=stdev, ~exp=2.0);
    let meanSquared = Js.Math.pow_float(~base=mean, ~exp=2.0);
    let mu =
      Js.Math.log(mean) -. 0.5 *. Js.Math.log(variance /. meanSquared +. 1.0);
    let sigma =
      Js.Math.pow_float(
        ~base=Js.Math.log(variance /. meanSquared +. 1.0),
        ~exp=0.5,
      );
    `Lognormal({mu, sigma});
  };

  let multiply = (l1, l2) => {
    let mu = l1.mu +. l2.mu;
    let sigma = l1.sigma +. l2.sigma;
    `Lognormal({mu, sigma});
  };
  let divide = (l1, l2) => {
    let mu = l1.mu -. l2.mu;
    let sigma = l1.sigma +. l2.sigma;
    `Lognormal({mu, sigma});
  };
  let operate = (operation: Operation.Algebraic.t, n1: t, n2: t) =>
    switch (operation) {
    | `Multiply => Some(multiply(n1, n2))
    | `Divide => Some(divide(n1, n2))
    | _ => None
    };
};

module Uniform = {
  type t = uniform;
  let pdf = (x, t: t) => Jstat.uniform##pdf(x, t.low, t.high);
  let cdf = (x, t: t) => Jstat.uniform##cdf(x, t.low, t.high);
  let inv = (p, t: t) => Jstat.uniform##inv(p, t.low, t.high);
  let sample = (t: t) => Jstat.uniform##sample(t.low, t.high);
  let mean = (t: t) => Ok(Jstat.uniform##mean(t.low, t.high));
  let toString = ({low, high}: t) => {j|Uniform($low,$high)|j};
  let truncate = (low, high, t: t): t => {
    let newLow = max(E.O.default(neg_infinity, low), t.low);
    let newHigh = min(E.O.default(infinity, high), t.high);
    {low: newLow, high: newHigh};
  };
};

module Float = {
  type t = float;
  let pdf = (x, t: t) => x == t ? 1.0 : 0.0;
  let cdf = (x, t: t) => x >= t ? 1.0 : 0.0;
  let inv = (p, t: t) => p < t ? 0.0 : 1.0;
  let mean = (t: t) => Ok(t);
  let sample = (t: t) => t;
  let toString = Js.Float.toString;
};

module T = {
  let minCdfValue = 0.0001;
  let maxCdfValue = 0.9999;

  let pdf = (x, dist) =>
    switch (dist) {
    | `Normal(n) => Normal.pdf(x, n)
    | `Triangular(n) => Triangular.pdf(x, n)
    | `Exponential(n) => Exponential.pdf(x, n)
    | `Cauchy(n) => Cauchy.pdf(x, n)
    | `Lognormal(n) => Lognormal.pdf(x, n)
    | `Pareto(n) => Pareto.pdf(x, n)
    | `Uniform(n) => Uniform.pdf(x, n)
    | `Beta(n) => Beta.pdf(x, n)
    | `Float(n) => Float.pdf(x, n)
    };

  let cdf = (x, dist) =>
    switch (dist) {
    | `Normal(n) => Normal.cdf(x, n)
    | `Triangular(n) => Triangular.cdf(x, n)
    | `Exponential(n) => Exponential.cdf(x, n)
    | `Cauchy(n) => Cauchy.cdf(x, n)
    | `Lognormal(n) => Lognormal.cdf(x, n)
    | `Pareto(n) => Pareto.cdf(x, n)
    | `Uniform(n) => Uniform.cdf(x, n)
    | `Beta(n) => Beta.cdf(x, n)
    | `Float(n) => Float.cdf(x, n)
    };

  let inv = (x, dist) =>
    switch (dist) {
    | `Normal(n) => Normal.inv(x, n)
    | `Triangular(n) => Triangular.inv(x, n)
    | `Exponential(n) => Exponential.inv(x, n)
    | `Cauchy(n) => Cauchy.inv(x, n)
    | `Lognormal(n) => Lognormal.inv(x, n)
    | `Pareto(n) => Pareto.inv(x, n)
    | `Uniform(n) => Uniform.inv(x, n)
    | `Beta(n) => Beta.inv(x, n)
    | `Float(n) => Float.inv(x, n)
    };

  let sample: symbolicDist => float =
    fun
    | `Normal(n) => Normal.sample(n)
    | `Triangular(n) => Triangular.sample(n)
    | `Exponential(n) => Exponential.sample(n)
    | `Cauchy(n) => Cauchy.sample(n)
    | `Lognormal(n) => Lognormal.sample(n)
    | `Uniform(n) => Uniform.sample(n)
    | `Beta(n) => Beta.sample(n)
    | `Pareto(n) => Pareto.sample(n)
    | `Float(n) => Float.sample(n);

  let doN = (n, fn) => {
    let items = Belt.Array.make(n, 0.0);
    for (x in 0 to n - 1) {
      let _ = Belt.Array.set(items, x, fn());
      ();
    };
    items;
  };

  let sampleN = (n, dist) => {
    doN(n, () => sample(dist));
  };

  let toString: symbolicDist => string =
    fun
    | `Triangular(n) => Triangular.toString(n)
    | `Exponential(n) => Exponential.toString(n)
    | `Cauchy(n) => Cauchy.toString(n)
    | `Normal(n) => Normal.toString(n)
    | `Lognormal(n) => Lognormal.toString(n)
    | `Uniform(n) => Uniform.toString(n)
    | `Beta(n) => Beta.toString(n)
    | `Pareto(n) => Pareto.toString(n)
    | `Float(n) => Float.toString(n);

  let min: symbolicDist => float =
    fun
    | `Triangular({low}) => low
    | `Exponential(n) => Exponential.inv(minCdfValue, n)
    | `Cauchy(n) => Cauchy.inv(minCdfValue, n)
    | `Normal(n) => Normal.inv(minCdfValue, n)
    | `Lognormal(n) => Lognormal.inv(minCdfValue, n)
    | `Uniform({low}) => low
    | `Pareto(n) => 0.0
    | `Beta(n) => Beta.inv(minCdfValue, n)
    | `Float(n) => n;

  let max: symbolicDist => float =
    fun
    | `Triangular(n) => n.high
    | `Exponential(n) => Exponential.inv(maxCdfValue, n)
    | `Cauchy(n) => Cauchy.inv(maxCdfValue, n)
    | `Normal(n) => Normal.inv(maxCdfValue, n)
    | `Lognormal(n) => Lognormal.inv(maxCdfValue, n)
    | `Beta(n) => Beta.inv(maxCdfValue, n)
    | `Pareto(n) => Pareto.inv(maxCdfValue, n)
    | `Uniform({high}) => high
    | `Float(n) => n;

  let mean: symbolicDist => result(float, string) =
    fun
    | `Triangular(n) => Triangular.mean(n)
    | `Exponential(n) => Exponential.mean(n)
    | `Cauchy(n) => Cauchy.mean(n)
    | `Normal(n) => Normal.mean(n)
    | `Pareto(n) => Pareto.mean(n)
    | `Lognormal(n) => Lognormal.mean(n)
    | `Beta(n) => Beta.mean(n)
    | `Uniform(n) => Uniform.mean(n)
    | `Float(n) => Float.mean(n);

  let operate = (distToFloatOp: ExpressionTypes.distToFloatOperation, s) =>
    switch (distToFloatOp) {
    | `Cdf(f) => Ok(cdf(f, s))
    | `Pdf(f) => Ok(pdf(f, s))
    | `Inv(f) => Ok(inv(f, s))
    | `Sample => Ok(sample(s))
    | `Mean => mean(s)
    };

  let interpolateXs =
      (~xSelection: [ | `Linear | `ByWeight]=`Linear, dist: symbolicDist, n) => {
    switch (xSelection, dist) {
    | (`Linear, _) => E.A.Floats.range(min(dist), max(dist), n)
    | (`ByWeight, `Uniform(n)) =>
      // In `ByWeight mode, uniform distributions get special treatment because we need two x's
      // on either side for proper rendering (just left and right of the discontinuities).
      let dx = 0.00001 *. (n.high -. n.low);
      [|n.low -. dx, n.low +. dx, n.high -. dx, n.high +. dx|];
    | (`ByWeight, _) =>
      let ys = E.A.Floats.range(minCdfValue, maxCdfValue, n);
      ys |> E.A.fmap(y => inv(y, dist));
    };
  };

  /* Calling e.g. "Normal.operate" returns an optional that wraps a result.
     If the optional is None, there is no valid analytic solution. If it Some, it
     can still return an error if there is a serious problem,
     like in the case of a divide by 0.
     */
  let tryAnalyticalSimplification =
      (
        d1: symbolicDist,
        d2: symbolicDist,
        op: ExpressionTypes.algebraicOperation,
      )
      : analyticalSimplificationResult =>
    switch (d1, d2) {
    | (`Float(v1), `Float(v2)) =>
      switch (Operation.Algebraic.applyFn(op, v1, v2)) {
      | Ok(r) => `AnalyticalSolution(`Float(r))
      | Error(n) => `Error(n)
      }
    | (`Normal(v1), `Normal(v2)) =>
      Normal.operate(op, v1, v2)
      |> E.O.dimap(r => `AnalyticalSolution(r), () => `NoSolution)
    | (`Lognormal(v1), `Lognormal(v2)) =>
      Lognormal.operate(op, v1, v2)
      |> E.O.dimap(r => `AnalyticalSolution(r), () => `NoSolution)
    | _ => `NoSolution
    };

  let toShape = (sampleCount, d: symbolicDist): DistTypes.shape =>
    switch (d) {
    | `Float(v) =>
      Discrete(
        Discrete.make(
          ~integralSumCache=Some(1.0),
          {xs: [|v|], ys: [|1.0|]},
        ),
      )
    | _ =>
      let xs = interpolateXs(~xSelection=`ByWeight, d, sampleCount);
      let ys = xs |> E.A.fmap(x => pdf(x, d));
      Continuous(Continuous.make(~integralSumCache=Some(1.0), {xs, ys}));
    };
};
