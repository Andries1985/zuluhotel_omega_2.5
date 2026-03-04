///////////////////////////////////////////////////////////
//
// Layers.inc
// 
// Constants for all the layers
///////////////////////////////////////////////////////////

const LAYER_NONE        :=  0x00;   // Not used?
const LAYER_HAND1       :=  0x01;   // One handed weapon.
const LAYER_HAND2       :=  0x02;   // Two handed weapon, shield || misc.
const LAYER_SHOES       :=  0x03;   // Shoes.
const LAYER_PANTS       :=  0x04;   // Pants.
const LAYER_SHIRT       :=  0x05;   // Shirt.
const LAYER_HELM        :=  0x06;   // Helm || Hat.
const LAYER_GLOVES      :=  0x07;   // Gloves.
const LAYER_RING        :=  0x08;   // Ring.
const LAYER_TALISMAN    :=  0x09;   // Talisman.
const LAYER_NECK        :=  0x0a;   // Neck.
const LAYER_HAIR        :=  0x0b;   // Hair
const LAYER_WAIST       :=  0x0c;   // Waist (half apron).
const LAYER_CHEST       :=  0x0d;   // Torso (inner) (chest armor).
const LAYER_WRIST       :=  0x0e;   // Bracelet.
const LAYER_PACK2       :=  0x0f;   // Unused (backpack, but ord. bp is 0x15).
const LAYER_BEARD       :=  0x10;   // Facial hair.
const LAYER_TUNIC       :=  0x11;   // Torso (middle) (tunic, sash etc.).
const LAYER_EARS        :=  0x12;   // Earrings.
const LAYER_ARMS        :=  0x13;   // Arms.
const LAYER_CAPE        :=  0x14;   // Back (cloak).
const LAYER_PACK        :=  0x15;   // Backpack
const LAYER_ROBE        :=  0x16;   // Torso (outer) (robe)
const LAYER_SKIRT       :=  0x17;   // Legs (outer) (skirt/robe).
const LAYER_LEGS        :=  0x18;   // Legs (inner) (leg armor).
const LAYER_MOUNT       :=  0x19;   // Mount (horse, ostard etc.).
const LAYER_VENDORSTOCK :=  0x1a;   // This vendor will sell and restock.
const LAYER_VENDOREXTRA :=  0x1b;   // This vendor will resell to players but not restock.
const LAYER_VENDORBUY   :=  0x1c;   // This vendor can buy from players but does not stock.
const LAYER_BANKBOX     :=  0x1d;   // Contents of bankbox
const LAYER_TRADE       :=  0x1e;   // Can be multiple of these, do not use directly.