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

    node_names = data["nodes"] |> Enum.map(&Map.get(&1, "name"))
    arg_names = data["args"] |> Enum.map(&Map.get(&1, "name"))

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
    evald = EEx.eval_file(Path.join(@templates_dir, "template.c.eex"), opts)

    File.write!("/tmp/corpus.h", evald)
    {_, 0} = System.cmd("gcc", ["-o-", "/tmp/corpus.h"])
    File.cp!("/tmp/corpus.h", "c_src/corpus.h")
  end
end
