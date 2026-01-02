#include "Template.hpp"

#include "terrain/World.hpp"
#include "PlacementSettings.hpp"


void Template::addBlocksToWorld(World& world, Pos3D origin, Template::ITemplateProcessor* processor,
                                PlacementSettings& settings, int flags) {
    if ((!this->blocks.empty() || !settings.getIgnoreEntities() && !this->entities.empty()) &&
        this->size.getX() >= 1 && this->size.getY() >= 1 && this->size.getZ() >= 1) {
        std::optional<lce::BlockState> block = settings.getReplacedBlock();
        std::optional<BoundingBox> structureBB = settings.getBoundingBox();

        for (BlockInfo& template$blockInfo: this->blocks) {
            Pos3D blockPos = transformedBlockPos(settings, template$blockInfo.pos).add(origin);
            std::optional<BlockInfo> template$blockInfo1 =
                    processor != nullptr ? processor->processBlock(world, blockPos, template$blockInfo)
                                         : template$blockInfo;

            if (template$blockInfo1 != std::nullopt) {
                lce::BlockState block1 = template$blockInfo1.value().blockState;

                if ((block == std::nullopt || block != block1) &&
                    /*(!settings.getIgnoreStructureBlock() || block1 != lce::BlocksInit::STRUCTURE_BLOCK.getDefaultState()) &&*/
                    (structureBB == std::nullopt || structureBB.value().isVecInside(blockPos))) {

                    lce::BlockState iBlockState = template$blockInfo1.value().blockState;
                    lce::BlockState iBlockState1 = iBlockState;
                    // lce::BlockState iBlockState = template$blockInfo1.blockState.withMirror(settings.getMirror());
                    // lce::BlockState iBlockState1 = iBlockState.withRotation(settings.getRotation());

                    /*
                    if (template$blockInfo1.tileEntityData != null) {
                        TileEntity tileentity = world.getTileEntity(blockPos);

                        if (tileentity != null) {
                            if (tileentity instanceof IInventory) { ((IInventory) tileentity).clear(); }

                            world.setBlockState(blockPos, Blocks.BARRIER.getDefaultState(), 4);
                        }
                    }
                    */

                    world.setBlock(blockPos, iBlockState1/*, flags*/);
                    // if (world.setBlock(blockPos, iBlockState1/*, flags*/)/* &&
                    // template$blockInfo1.tileEntityData != std::nullopt*/) {
                    /*
                    TileEntity tileEntity2 = world.getTileEntity(blockPos);

                    if (tileEntity2 != std::nullopt) {
                        template$blockInfo1.tileEntityData.setInteger("x", blockPos.getX());
                        template$blockInfo1.tileEntityData.setInteger("y", blockPos.getY());
                        template$blockInfo1.tileEntityData.setInteger("z", blockPos.getZ());
                        tileEntity2.readFromNBT(template$blockInfo1.tileEntityData);
                        tileEntity2.mirror(settings.getMirror());
                        tileEntity2.rotate(settings.getRotation());
                    }
                    */
                    // }
                }
            }
        }

        /*
        for (BlockInfo template$blockInfo2: this->blocks) {
            if (block == std::nullopt || block != template$blockInfo2.blockState) {
                Pos3D blockPos1 = transformedBlockPos(settings, template$blockInfo2.pos).add(origin);

                if (structureBB == std::nullopt || structureBB.value().isVecInside(blockPos1)) {
                    world.notifyNeighborsRespectDebug(blockPos1, template$blockInfo2.blockState, false);

                    if (template$blockInfo2.tileEntityData != null) {
                        TileEntity tileentity1 = world.getTileEntity(blockPos1);

                        if (tileentity1 != null) { tileentity1.markDirty(); }
                    }
                }
            }
        }
        */

        /*
        if (!settings.getIgnoreEntities()) {
            this->addEntitiesToWorld(world, origin, settings.getMirror(), settings.getRotation(),
                                    structureBB);
        }
        */
    }
}


std::map<Pos3D, std::string> Template::getDataBlocks(const Pos3D pos, PlacementSettings& placementIn) {
    std::map<Pos3D, std::string> map = {};
    std::optional<BoundingBox> structureBB = placementIn.getBoundingBox();

    for (const BlockInfo& template$blockInfo: this->blocks) {
        Pos3D blockPos = transformedBlockPos(placementIn, template$blockInfo.pos).add(pos);

        if (structureBB == std::nullopt || structureBB.value().isVecInside(blockPos)) {
            lce::BlockState iBlockState = template$blockInfo.blockState;

            if (iBlockState == lce::BlocksInit::STRUCTURE_BLOCK.getDefaultState()
                /*&& template$blockInfo.tileEntityData != std::nullopt*/) {
                /*
                TileEntityStructure::Mode tileEntityStructure$mode =
                        TileEntityStructure::Mode::valueOf(template$blockInfo.tileEntityData.getString("mode"));

                if (tileEntityStructure$mode == TileEntityStructure::Mode::DATA) {
                    map.insert(blockPos, template$blockInfo.tileEntityData.getString("metadata"));
                }
                */
            }
        }
    }

    return map;
}


Pos3D Template::calculateConnectedPos(const PlacementSettings& fromSettings, const Pos3D fromPos,
                                      const PlacementSettings& toSettings, Pos3D toPos) {
    const Pos3D blockPos = transformedBlockPos(fromSettings, fromPos);
    const Pos3D blockPos1 = transformedBlockPos(toSettings, toPos);
    return blockPos.subtract(blockPos1);
}


Pos3D Template::transformedBlockPos(const PlacementSettings& settings, const Pos3D localPos) {
    return transformedBlockPos(localPos, settings.getMirror(), settings.getRotation());
}


void Template::addBlocksToWorldChunk(World& worldIn, const Pos3D pos, PlacementSettings& placementIn) {
    placementIn.setBoundingBoxFromChunk();
    this->addBlocksToWorld(worldIn, pos, placementIn);
}


void Template::addBlocksToWorld(World& worldIn, Pos3D pos, PlacementSettings& placementIn) {
    // auto processor = BlockRotationProcessor(pos, placementIn);
    this->addBlocksToWorld(worldIn, pos, nullptr, placementIn, 2);
}


void Template::addBlocksToWorld(World& worldIn, Pos3D pos, PlacementSettings& placementIn, const int flags) {
    auto processor = BlockRotationProcessor(pos, placementIn);
    this->addBlocksToWorld(worldIn, pos, &processor, placementIn, flags);
}


void Template::addEntitiesToWorld(World& worldIn, const Pos3D pos, const Mirror mirrorIn, const Rotation rotationIn,
                                  const BoundingBox aabb) {
    for (const EntityInfo& template$entityinfo: this->entities) {
        Pos3D blockPos = transformedBlockPos(template$entityinfo.blockPos, mirrorIn, rotationIn).add(pos);

        if (/*aabb == null || */aabb.isVecInside(blockPos)) {
            /*
            NBTCompound nBTTagCompound = template$entityinfo.entityData;
            Pos3DTemplate<double> vec3d =
                    transformedBlockPos<double>(template$entityinfo.pos, mirrorIn, rotationIn);
            Pos3DTemplate<double> vec3d1 = vec3d.add(pos.getX(), pos.getY(), pos.getZ());
            NBTList nbtTagList = NBTList(eNBT::DOUBLE); // new NBTList();
            nbtTagList.push_back(makeDouble(vec3d1.x));
            nbtTagList.push_back(makeDouble(vec3d1.y));
            nbtTagList.push_back(makeDouble(vec3d1.z));
            nBTTagCompound.insert("Pos", nbtTagList);
            nBTTagCompound.setUniqueId("UUID", UUID.randomUUID());
            Entity entity;

            try {
                entity = EntityList.createEntityFromNBT(nBTTagCompound, worldIn);
            } catch (Exception var15) { entity = null; }

            if (entity != null) {
                float f = entity.getMirroredYaw(mirrorIn);
                f = f + (entity.rotationYaw - entity.getRotatedYaw(rotationIn));
                entity.setLocationAndAngles(vec3d1.xCoord, vec3d1.yCoord, vec3d1.zCoord, f, entity.rotationPitch);
                worldIn.spawnEntityInWorld(entity);
            }
            */
        }
    }
}


Pos3D Template::transformedSize(const Rotation rotationIn) const {
    switch (rotationIn.type) {
        case Rotation::Type::COUNTERCLOCKWISE_90:
        case Rotation::Type::CLOCKWISE_90:
            return {this->size.getZ(), this->size.getY(), this->size.getX()};

        default:
            return this->size;
    }
}


Pos3D Template::transformedBlockPos(const Pos3D pos, const Mirror mirrorIn, const Rotation rotationIn) {
    int x = pos.getX();
    int y = pos.getY();
    int z = pos.getZ();
    bool flag = true;

    switch (mirrorIn.type) {
        case Mirror::Type::LEFT_RIGHT:
            z = -z;
            break;

        case Mirror::Type::FRONT_BACK:
            x = -x;
            break;

        default:
            flag = false;
    }

    switch (rotationIn.type) {
        case Rotation::Type::COUNTERCLOCKWISE_90:
            return {z, y, -x};

        case Rotation::Type::CLOCKWISE_90:
            return {-z, y, x};

        case Rotation::Type::CLOCKWISE_180:
            return {-x, y, -z};

        default:
            return flag ? Pos3D(x, y, z) : pos;
    }
}


Pos3DTemplate<double> Template::transformedBlockPos(Pos3DTemplate<double> vec, Mirror mirrorIn, Rotation rotationIn) {
    double dx = vec.x;
    double dy = vec.y;
    double dz = vec.z;
    bool flag = true;

    switch (mirrorIn.type) {
        case Mirror::Type::LEFT_RIGHT:
            dz = 1.0 - dz;
            break;

        case Mirror::Type::FRONT_BACK:
            dx = 1.0 - dx;
            break;

        default:
            flag = false;
    }

    switch (rotationIn.type) {
        case Rotation::Type::COUNTERCLOCKWISE_90:
            return {dz, dy, 1.0 - dx};

        case Rotation::Type::CLOCKWISE_90:
            return {1.0 - dz, dy, dx};

        case Rotation::Type::CLOCKWISE_180:
            return {1.0 - dx, dy, 1.0 - dz};

        default:
            return flag ? Pos3DTemplate<double>(dx, dy, dz) : vec;
    }
}


Pos3D Template::getZeroPositionWithTransform(const Pos3D origin, const Mirror mirror, const Rotation rotation) const {
    return transformOrigin(origin, mirror, rotation, this->getSize().getX(), this->getSize().getZ());
}


Pos3D Template::transformOrigin(const Pos3D origin, const Mirror mirror, const Rotation rotation, int width,
                                int depth) {
    --width;
    --depth;
    const int i = mirror == Mirror::FRONT_BACK ? width : 0;
    const int j = mirror == Mirror::LEFT_RIGHT ? depth : 0;
    Pos3D blockPos = origin;

    switch (rotation.type) {
        case Rotation::Type::COUNTERCLOCKWISE_90:
            blockPos = origin.add(j, 0, width - i);
            break;

        case Rotation::Type::CLOCKWISE_90:
            blockPos = origin.add(depth - j, 0, i);
            break;

        case Rotation::Type::CLOCKWISE_180:
            blockPos = origin.add(width - i, 0, depth - j);
            break;

        case Rotation::Type::NONE:
            blockPos = origin.add(i, 0, j);
    }

    return blockPos;
}


void Template::registerTemplateDataFixers() {
    /*
    dataFixer.registerWalker(FixTypes.STRUCTURE, new IDataWalker() {
        public NBTCompound process(IDataFixer fixer, NBTCompound compound, int versionIn) {
            if (compound.hasKey("entities", 9)) {
                NBTList nBTTagList = compound.getTagList("entities", 10);

                    for (int i = 0; i < nBTTagList.tagCount(); ++i) {
            NBTCompound nBTTagCompound = (NBTCompound) nBTTagList.get(i);

            if (nBTTagCompound.hasKey("nbt", 10)) {
                nBTTagCompound.setTag("nbt",
                                      fixer.process(FixTypes.ENTITY, nBTTagCompound.getCompoundTag("nbt"), versionIn));
            }
        }

    }

    if (compound.hasKey("blocks", 9)) {
        NBTList nBTTagList1 = compound.getTagList("blocks", 10);

        for (int j = 0; j < nBTTagList1.tagCount(); ++j) {
            NBTCompound nBTTagCompound1 = (NBTCompound) nBTTagList1.get(j);

            if (nBTTagCompound1.hasKey("nbt", 10)) {
                nBTTagCompound1.setTag(
                        "nbt", fixer.process(FixTypes.BLOCK_ENTITY, nBTTagCompound1.getCompoundTag("nbt"), versionIn));
            }
        }
    }

    return compound;
    }
});
    */
}


bool Template::readPosXYZ(const NBTBase& tag, int& x, int& y, int& z) {
    // Supports either INT_ARRAY [x,y,z] or LIST of 3 ints
    if (tag.is<NBTIntArray>()) {
        const auto& arr = tag.get<NBTIntArray>();
        if (arr.size() < 3) return false;
        x = arr[0]; y = arr[1]; z = arr[2];
        return true;
    }
    if (tag.is<NBTList>()) {
        const auto& li = tag.get<NBTList>();
        if (li.size() < 3) return false;
        x = li[0].get<i32>();
        y = li[1].get<i32>();
        z = li[2].get<i32>();
        return true;
    }
    return false;
}


void Template::read(NBTCompound& compound) {
    this->blocks.clear();
    this->entities.clear();

    auto sizeTag = compound("size").get<NBTList>();
    this->size = Pos3D(sizeTag[0].get<i32>(), sizeTag[1].get<i32>(), sizeTag[2].get<i32>());

    // this->author = compound("author").get<eNBT::STRING>();
    BasicPalette template$basicPalette{};

    const NBTBase& paletteTag = compound("palette");
    if (!paletteTag || !paletteTag.is<NBTList>()) {
        throw std::runtime_error("palette missing or not a list");
    }
    const auto& nBTTagList1 = paletteTag.get<NBTList>();

    // Build palette with proper indexing
    for (size_t i = 0; i < nBTTagList1.size(); ++i) {
        const auto& entry = nBTTagList1[i];

        std::string name = entry("Name").is<std::string>() ? entry("Name").get<std::string>() : std::string{};
        if (name.empty() && entry.is<NBTCompound>()) {
            if (auto opt = entry.get<NBTCompound>().value<std::string>("Name")) {
                name = *opt;
            }
        }

        // Remove "minecraft:" prefix if present
        if (name.starts_with("minecraft:")) {
            name = name.substr(10);
        }

        auto* block = lce::registry::BlockRegistry::getBlockFromName(name);
        if (block == nullptr) {
            std::cerr << "Warning: Unknown block '" << name << "' in template, using AIR" << std::endl;
            block = &lce::BlocksInit::AIR;
        }

        // Use the loop index as the palette ID
        template$basicPalette.addMapping(block->getDefaultState(), static_cast<int>(i));
    }

    const NBTBase& blocksTag = compound("blocks");
    if (!blocksTag || !blocksTag.is<NBTList>()) {
        throw std::runtime_error("blocks missing or not a list");
    }
    const auto& blockList = blocksTag.get<NBTList>();

    for (const auto& b : blockList) {
        if (!b.is<NBTCompound>()) continue;

        auto block = b.get<NBTCompound>();

        auto posTag = b("pos").get<NBTList>();
        Pos3D blockPos(posTag[0].get<i32>(), posTag[1].get<i32>(), posTag[2].get<i32>());

        int stateId = block.getOr("state", -1);
        lce::BlockState iBlockState = template$basicPalette.stateFor(stateId);

        NBTCompound nBTTagCompound1;
        if (block("nbt")) {
            nBTTagCompound1 = block["nbt"].get<NBTCompound>();
        } else {
            nBTTagCompound1 = makeCompound().get<NBTCompound>();
        }

        this->blocks.emplace_back(blockPos, iBlockState, nBTTagCompound1);
    }

    /*
    NBTList nBTTagList4 = compound.getTagList("entities", 10);

    for (int k = 0; k < nBTTagList4.tagCount(); ++k) {
        NBTCompound nBTTagCompound3 = nBTTagList4.getCompoundTagAt(k);
        NBTList nBTTagList5 = nBTTagCompound3.getTagList("pos", 6);
        Pos3DTemplate<double> vec3d = new Pos3DTemplate<double>(nBTTagList5.getDoubleAt(0), nBTTagList5.getDoubleAt(1), nBTTagList5.getDoubleAt(2));
        NBTList nBTTagList6 = nBTTagCompound3.getTagList("blockPos", 3);
        Pos3D blockpos1 = Pos3D(nBTTagList6.getIntAt(0), nBTTagList6.getIntAt(1), nBTTagList6.getIntAt(2));

        if (nBTTagCompound3.hasKey("nbt")) {
            NBTCompound nBTTagCompound2 = nBTTagCompound3.getCompoundTag("nbt");
            this->entities.add(new Template::EntityInfo(vec3d, blockpos1, nBTTagCompound2));
        }
    }
    */
}
