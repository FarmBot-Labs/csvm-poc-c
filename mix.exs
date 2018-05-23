defmodule Csvm.MixProject do
  use Mix.Project

  def project do
    [
      app: :csvm,
      version: "0.1.0",
      elixir: "~> 1.6",
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_clean: ["clean"],
      make_env: make_env(),
      # package: package(),
      # description: description(),
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      aliases: [format: [&format_c/1, "format"]]
    ]
  end

  defp make_env() do
    case System.get_env("ERL_EI_INCLUDE_DIR") do
      nil ->
        %{
          "DEBUG" => "1",
          "ERL_INCLUDE_DIR" => "#{:code.root_dir()}/usr/include",
          "ERL_LIBDIR" => "#{:code.root_dir()}/usr/lib"
        }

      _ ->
        %{}
    end
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      mod: {Csvm.Application, []}
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:dialyxir, "~> 0.5.1", runtime: false},
      {:elixir_make, "~> 0.4.1"},
      {:ex_doc, "~> 0.18.1", only: [:dev, :test]},
      {:poison, "~> 3.1"}
    ]
  end

  defp format_c([]) do
    astyle =
      System.find_executable("astyle") ||
        Mix.raise("""
        Could not format C code since astyle is not available.
        """)

    System.cmd(
      astyle,
      ["-n", "c_src/*.c", "c_src/*.h"],
      into: IO.stream(:stdio, :line)
    )
  end

  defp format_c(_args), do: true
end
