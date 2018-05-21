defmodule Csvm.RequestPacket do
  alias Csvm.RequestPacket
  defstruct [:request_id, :namespace, :command, :payload]

  @typedoc "Request id is a 16 bit integer."
  @type request_id :: 0..65535

  @typedoc """
  Namespace for a packet. 4 Bytes.
  ## Examples
  * `SLCE` - slice of memory.
  * `REGS` - Register
  * `CODE` - AST loading, deloading etc.
  * `PROC` - Process management.
  * `SSYS` - System Control and hypercalls.
  """
  @type namespace :: <<_::32>>

  @typedoc """
  Command for a given `namespace`.
  ## Examples
  ### `CODE` namespace
  * `OPEN` - Open a code buffer
  * `CLSE` - Close a buffer.
  """
  @type command :: binary

  @typedoc """
  Variable length Payload. Depends on command.
  """
  @type payload :: binary

  @typedoc "Request Packet type."
  @type t :: %RequestPacket{
          request_id: request_id,
          namespace: namespace,
          command: command,
          payload: payload
        }

  @doc "Decode a packet."
  @spec decode(binary) :: t
  def decode(binary_data)

  def decode(<<
        request_id::big-integer-size(16),
        namespace::binary-size(4),
        command::binary-size(5),
        payload_num_bytes::big-integer-size(16),
        "\r\n",
        payload::binary-size(payload_num_bytes)
      >>) do
    struct(
      RequestPacket,
      namespace: namespace,
      command: command,
      payload: payload,
      request_id: request_id
    )
  end

  @doc "Encode a packet."
  @spec encode(integer, namespace, command, payload) :: binary
  def encode(request_id, namespace, command, payload)

  def encode(request_id, <<namespace::binary-size(4)>>, <<command::binary-size(5)>>, payload)
      when request_id <= 65535 and is_binary(payload) do
    payload_num_bytes = byte_size(payload)

    <<request_id::size(16)>> <>
      namespace <> command <> <<payload_num_bytes::size(16)>> <> "\r\n" <> payload
  end

  @doc "Encode a packet."
  @spec encode(t) :: binary
  def encode(%RequestPacket{} = packet) do
    encode(packet.request_id, packet.namespace, packet.command, packet.payload)
  end
end
