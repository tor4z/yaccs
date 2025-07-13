import torch
from torch import nn


class Model(nn.Module):
    def __init__(self):
        super().__init__()
        # Define the layers of the network
        self.fc1 = nn.Linear(4, 8)  # First fully connected layer
        self.relu = nn.ReLU()                          # ReLU activation function
        self.fc2 = nn.Linear(8, 4) # Second fully connected layer

    def forward(self, x):
        # Define the forward pass (how data flows through the network)
        x = self.fc1(x)       # Apply the first linear transformation
        x = self.relu(x)      # Apply the ReLU activation
        x = self.fc2(x)       # Apply the second linear transformation
        return x


if __name__ == "__main__":
    ONNX_PATH = "model.onnx"

    dummy_input = torch.randn(1, 4)
    model = Model()

    print(model.forward(dummy_input))

    torch.onnx.export(
        model,
        dummy_input,
        ONNX_PATH,
        export_params=True,
        opset_version=20,
        do_constant_folding=True,
        input_names=["input"],
        output_names=["output"],
        dynamic_axes={
            "input": {0: "batch_size"},
            "output": {0: "batch_size"},
        }
    )
