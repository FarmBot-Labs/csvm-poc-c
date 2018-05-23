defmodule NifTest do
  require Logger
  @on_load :load_nif
  @doc false
  def load_nif do
    nif_file = '#{:code.priv_dir(:csvm)}/nif_test'
    case :erlang.load_nif(nif_file, 0) do
      :ok -> :ok
      {:error, {:reload, _}} -> :ok
      {:error, reason} -> Logger.warn "Failed to load nif: #{inspect reason}"
    end
  end

  def function(), do: :error
end
