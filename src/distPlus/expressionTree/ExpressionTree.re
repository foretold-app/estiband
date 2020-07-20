open ExpressionTypes.ExpressionTree;

let rec toString =
  fun
  | `SymbolicDist(d) => SymbolicDist.T.toString(d)
  | `RenderedDist(_) => "[shape]"
  | op => Operation.T.toString(toString, op);

let toShape = (intendedShapeLength: int, samplingInputs, node: node) => {
  // Js.log(toString(node));
  let renderResult =
    `Render(`Normalize(node))
    |> ExpressionTreeEvaluator.toLeaf({
         samplingInputs,
         intendedShapeLength,
         evaluateNode: ExpressionTreeEvaluator.toLeaf,
       });

  switch (renderResult) {
  | Ok(`RenderedDist(shape)) => Ok(shape)
  | Ok(_) => Error("Rendering failed.")
  | Error(e) => Error(e)
  };
};
