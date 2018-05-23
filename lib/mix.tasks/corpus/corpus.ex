defmodule Mix.Tasks.Corpus do
  @moduledoc """
  Generate constants for things based off the corpus.
  """

  use Mix.Task
  @templates_dir __DIR__

  def run([file]) do
    data =
      File.read!(file)
      |> Poison.decode!()

    node_names =
      data["nodes"]
      |> Enum.map(&Map.get(&1, "name"))
      |> Enum.map(&Macro.underscore(Macro.camelize(&1)))

    arg_names =
      data["args"]
      |> Enum.map(&Map.get(&1, "name"))
      |> Enum.map(&Macro.underscore(Macro.camelize(&1)))

    node_names =
      Enum.reduce(0..(Enum.count(node_names) - 1), [], fn index, acc ->
        name = Enum.at(node_names, index)
        [%{index: index, upcase_name: String.upcase(name), name: name} | acc]
      end)
      |> Enum.reverse()

    arg_names =
      Enum.reduce(0..(Enum.count(arg_names) - 1), [], fn index, acc ->
        name = Enum.at(arg_names, index)
        [%{index: index, upcase_name: String.upcase(name), name: name} | acc]
      end)
      |> Enum.reverse()

    opts = [node_names: node_names, arg_names: arg_names, tag: data["tag"]]
    evald_c = EEx.eval_file(Path.join(@templates_dir, "corpus.c.eex"), opts)
    evald_h = EEx.eval_file(Path.join(@templates_dir, "corpus.h.eex"), opts)

    File.write!("c_src/corpus.c", evald_c)
    File.write!("c_src/corpus.h", evald_h)
  end
end
