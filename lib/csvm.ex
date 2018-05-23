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
    {:ok, %{port: port, context: :idle, from: nil}}
  end

  def terminate(reason, _) do
    Logger.warn("Crash: #{inspect(reason)}")
  end

  def handle_call({:command, packet}, from, state) do
    Port.command(state.port, packet)
    {:noreply, %{state | context: :sent, from: from}}
  end

  def handle_info({_port, {:data, data}}, %{context: :sent, from: from} = state) do
    resp = Csvm.ResponsePacket.decode(data)
    GenServer.reply(from, resp)
    {:noreply, %{state | context: :idle, from: nil}}
  end

  def handle_info({_port, {:data, data}}, state) do
    Logger.warn("unhandled data: #{inspect(data)}")
    {:noreply, state}
  end

  def handle_info({_, {:exit_status, status}}, _state) do
    Logger.warn("port exit: #{status}")
    {:noreply, %{port: open_port()}}
  end

  def open_port do
    executable = :code.priv_dir(:csvm) ++ '/csvm'

    Port.open({:spawn_executable, executable}, [
      {:args, []},
      :binary,
      :exit_status
    ])
  end
end
