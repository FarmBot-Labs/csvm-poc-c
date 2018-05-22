defmodule Csvm do
  use GenServer
  require Logger

  def start_link(_) do
    GenServer.start_link(__MODULE__, [], name: __MODULE__)
  end

  def command(packet) do
    GenServer.call(__MODULE__, {:command, packet})
  end

  def init(_) do
    port = open_port()
    {:ok, %{port: port}}
  end

  def handle_call({:command, packet}, _, state) do
    Port.command(state.port, packet)
    {:reply, :ok, state}
  end

  def handle_info({_port, {:data, data}}, state) do
    IO.inspect(data, label: "DATA")
    require IEx; IEx.pry
    {:noreply, state}
  end

  def handle_info({_, {:exit_status, status}}, state) do
    Logger.warn "port exit: #{status}"
    {:noreply, %{port: open_port()}}
  end

  def open_port do
    executable = :code.priv_dir(:csvm) ++ '/csvm'

    port =
      Port.open({:spawn_executable, executable}, [
        {:args, []},
        # {:packet, 2},
        :binary,
        :exit_status
      ])
    port
  end
end
