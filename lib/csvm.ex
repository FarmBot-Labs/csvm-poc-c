defmodule Csvm do
  use GenServer

  def start_link(_) do
    GenServer.start_link(__MODULE__, [], name: __MODULE__)
  end

  def init(_) do
    executable = :code.priv_dir(:csvm) ++ '/csvm'

    port =
      Port.open({:spawn_executable, executable}, [
        {:args, []},
        {:packet, 2},
        :binary,
        :exit_status
      ])

    {:ok, %{port: port}}
  end

  def handle_info({:port, _port, data}, state) do
    IO.inspect(data, label: "DATA")
    {:noreply, state}
  end
end
