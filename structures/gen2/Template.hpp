#pragma once

#include "common/Pos3DTemplate.hpp"
#include "common/nbt.hpp"
#include "lce/blocks/block.hpp"
#include "terrain/World.hpp"
#include <list>
#include <utility>
#include <unordered_map>

class Template {
public:
    
    class BlockInfo {
    public:
        const Pos3D pos;
        const lce::BlockState blockState;
        const NBTCompound tileEntityData;
    
        BlockInfo(const Pos3D posIn, const lce::BlockState stateIn, NBTCompound compoundIn)
                : pos(posIn), blockState(stateIn), tileEntityData(std::move(compoundIn)) {}
    };
    
    class EntityInfo {
    public:
        const Pos3DTemplate<double> pos;
        const Pos3D blockPos;
        const NBTCompound entityData;
    
        EntityInfo(const Pos3DTemplate<double>& vecIn, const Pos3D posIn, NBTCompound compoundIn)
            : pos(vecIn), blockPos(posIn), entityData(std::move(compoundIn)) {}
    };
    
    
private:
    std::list<BlockInfo> blocks = std::list<BlockInfo>();
    std::list<EntityInfo> entities = std::list<EntityInfo>();

    Pos3D size = {0, 0, 0};

    std::string author = "?";

                                                                                            
public:
    
    ND Pos3D getSize() const {
        return this->size;
    }

    void setAuthor(const std::string& authorIn) {
        this->author = authorIn;
    }

    ND std::string getAuthor() {
        return this->author;
    }

    /*
    void takeBlocksFromWorld(World worldIn, const Pos3D startPos, const Pos3D endPos, const bool takeEntities, lce::BlockState toIgnore) {
        if (endPos.getX() >= 1 && endPos.getY() >= 1 && endPos.getZ() >= 1) {
            const Pos3D blockPos = startPos.add(endPos).add(-1, -1, -1);
            std::list<BlockInfo> list = std::list<BlockInfo>();
            std::list<BlockInfo> list1 = std::list<BlockInfo>();
            std::list<BlockInfo> list2 = std::list<BlockInfo>();
            Pos3D blockPos1 =
                    Pos3D(std::min(startPos.getX(), blockPos.getX()), std::min(startPos.getY(), blockPos.getY()),
                                 std::min(startPos.getZ(), blockPos.getZ()));
            Pos3D blockPos2 =
                    Pos3D(std::max(startPos.getX(), blockPos.getX()), std::max(startPos.getY(), blockPos.getY()),
                                 std::max(startPos.getZ(), blockPos.getZ()));
            this->size = endPos;

            for (Pos3D blockPos$mutableBlockPos: BlockPos::getAllInBoxMutable(blockPos1, blockPos2)) {
                Pos3D blockpos3 = blockPos$mutableBlockPos.subtract(blockPos1);
                lce::BlockState iBlockState = worldIn.getBlockState(blockPos$mutableBlockPos);

                if (toIgnore == null || toIgnore != iBlockState.getBlock()) {
                    TileEntity tileentity = worldIn.getTileEntity(blockPos$mutableBlockPos);

                    if (tileentity != null) {
                        NBTCompound nBTTagCompound = tileentity.writeToNBT(new NBTCompound());
                        nBTTagCompound.removeTag("x");
                        nBTTagCompound.removeTag("y");
                        nBTTagCompound.removeTag("z");
                        list1.add(new Template::BlockInfo(blockpos3, iBlockState, nBTTagCompound));
                    } else if (!iBlockState.isFullBlock() && !iBlockState.isFullCube()) {
                        list2.add(new Template::BlockInfo(blockpos3, iBlockState, (NBTCompound) null));
                    } else {
                        list.add(new Template::BlockInfo(blockpos3, iBlockState, (NBTCompound) null));
                    }
                }
            }

            this->blocks.clear();
            this->blocks.addAll(list);
            this->blocks.addAll(list1);
            this->blocks.addAll(list2);

            if (takeEntities) {
                this->takeEntitiesFromWorld(worldIn, blockPos1, blockPos2.add(1, 1, 1));
            } else {
                this->entities.clear();
            }
        }
    }
    */

private:
    /*
    void takeEntitiesFromWorld(World& worldIn, Pos3D startPos, Pos3D endPos) {
        std::list<Entity> list = worldIn.<Entity>getEntitiesWithinAABB(
                Entity.class, new AxisAlignedBB(startPos, endPos), new Predicate<Entity>() {
                public
                    bool apply(@Nullable Entity entityCandidate) {
                        return !(entityCandidate instanceof EntityPlayer);
                    }
                });
        this->entities.clear();

        for (Entity entity: list) {
            Pos3DTemplate<double> vec3d = new Pos3DTemplate<double>(entity.posX - (double) startPos.getX(), entity.posY - (double) startPos.getY(),
                                    entity.posZ - (double) startPos.getZ());
            NBTCompound nBTTagCompound = new NBTCompound();
            entity.writeToNBTOptional(nBTTagCompound);
            Pos3D blockpos;

            if (entity instanceof EntityPainting) {
                blockpos = ((EntityPainting) entity).getHangingPosition().subtract(startPos);
            } else {
                blockpos = Pos3D(vec3d);
            }

            this->entities.add(new Template::EntityInfo(vec3d, blockpos, nBTTagCompound));
        }
    }
    */

public:
    std::map<Pos3D, std::string> getDataBlocks(const Pos3D pos, const PlacementSettings& placementIn) {
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

    static Pos3D calculateConnectedPos(const PlacementSettings& fromSettings, const Pos3D fromPos, const PlacementSettings& toSettings, Pos3D toPos) {
        const Pos3D blockPos = transformedBlockPos(fromSettings, fromPos);
        const Pos3D blockPos1 = transformedBlockPos(toSettings, toPos);
        return blockPos.subtract(blockPos1);
    }

    static Pos3D transformedBlockPos(const PlacementSettings& settings, const Pos3D localPos) {
        return transformedBlockPos(localPos, settings.getMirror(), settings.getRotation());
    }

    void addBlocksToWorldChunk(World& worldIn, const Pos3D pos, const PlacementSettings& placementIn) {
        placementIn.setBoundingBoxFromChunk();
        this->addBlocksToWorld(worldIn, pos, placementIn);
    }

    void addBlocksToWorld(World& worldIn, Pos3D pos, const PlacementSettings& placementIn) {
        this->addBlocksToWorld(worldIn, pos, new BlockRotationProcessor(pos, placementIn), placementIn, 2);
    }

    void addBlocksToWorld(World& worldIn, Pos3D pos, const PlacementSettings& placementIn, const int flags) {
        this->addBlocksToWorld(worldIn, pos, new BlockRotationProcessor(pos, placementIn), placementIn, flags);
    }

    void addBlocksToWorld(World& world, Pos3D origin, ITemplateProcessor processor, PlacementSettings settings, MU int flags) {
        if ((!this->blocks.empty() || !settings.getIgnoreEntities() && !this->entities.empty()) &&
            this->size.getX() >= 1 && this->size.getY() >= 1 && this->size.getZ() >= 1) {
            std::optional<lce::BlockState> block = settings.getReplacedBlock();
            std::optional<BoundingBox> structureBB = settings.getBoundingBox();

            for (BlockInfo template$blockInfo: this->blocks) {
                Pos3D blockPos = transformedBlockPos(settings, template$blockInfo.pos).add(origin);
                BlockInfo template$blockInfo1 =
                        processor != nullptr ? processor.processBlock(world, blockPos, template$blockInfo)
                                          : template$blockInfo;

                if (template$blockInfo1 != nullptr) {
                    lce::BlockState block1 = template$blockInfo1.blockState;

                    if ((block == std::nullopt || block != block1) &&
                        (!settings.getIgnoreStructureBlock() || block1 != lce::BlocksInit::STRUCTURE_BLOCK.getDefaultState()) &&
                        (structureBB == std::nullopt || structureBB.value().isVecInside(blockPos))) {

                        lce::BlockState iBlockState = template$blockInfo1.blockState;
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

                        if (world.setBlock(blockPos, iBlockState1/*, flags*/)/* &&
                            template$blockInfo1.tileEntityData != std::nullopt*/) {
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
                        }
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

private:
    
    void addEntitiesToWorld(MU World& worldIn, const Pos3D pos, const Mirror mirrorIn, const Rotation rotationIn, const BoundingBox aabb) {
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
public:
    ND Pos3D transformedSize(const Rotation rotationIn) const {
        switch (rotationIn.type) {
            case Rotation::Type::COUNTERCLOCKWISE_90:
            case Rotation::Type::CLOCKWISE_90:
                return {this->size.getZ(), this->size.getY(), this->size.getX()};

            default:
                return this->size;
        }
    }
private:
    static Pos3D transformedBlockPos(const Pos3D pos, const Mirror mirrorIn, const Rotation rotationIn) {
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

    static Pos3DTemplate<double> transformedBlockPos(Pos3DTemplate<double> vec, Mirror mirrorIn, Rotation rotationIn) {
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
public:
    ND Pos3D getZeroPositionWithTransform(const Pos3D origin, const Mirror mirror, const Rotation rotation) const {
        return transformOrigin(origin, mirror, rotation, this->getSize().getX(), this->getSize().getZ());
    }

    static Pos3D transformOrigin(const Pos3D origin, const Mirror mirror, const Rotation rotation, int width, int depth) {
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

    static void registerTemplateDataFixers(/*DataFixer dataFixer*/) {
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

public:
    /*
    NBTCompound writeToNBT(NBTCompound nbt) {
        Template::BasicPalette template$basicPalette = new Template::BasicPalette();
        NBTList nBTTagList = new NBTList();
    
        for (Template::BlockInfo template$blockInfo: this->blocks) {
            NBTCompound nBTTagCompound = new NBTCompound();
            nBTTagCompound.setTag("pos", this->writeInts(template$blockInfo.pos.getX(), template$blockInfo.pos.getY(),
                                                        template$blockInfo.pos.getZ()));
            nBTTagCompound.setInteger("state", template$basicPalette.idFor(template$blockInfo.blockState));
    
            if (template$blockInfo.tileEntityData != null) {
                nBTTagCompound.setTag("nbt", template$blockInfo.tileEntityData);
            }
    
            nBTTagList.appendTag(nBTTagCompound);
        }
    
        NBTList nBTTagList1 = new NBTList();
    
        for (Template::EntityInfo template$entityinfo: this->entities) {
            NBTCompound nBTTagCompound1 = new NBTCompound();
            nBTTagCompound1.setTag("pos", this->writeDoubles(template$entityinfo.pos.xCoord, template$entityinfo.pos.yCoord,
                                                            template$entityinfo.pos.zCoord));
            nBTTagCompound1.setTag("blockPos",
                                   this->writeInts(template$entityinfo.blockPos.getX(), template$entityinfo.blockPos.getY(),
                                                  template$entityinfo.blockPos.getZ()));
    
            if (template$entityinfo.entityData != null) { nBTTagCompound1.setTag("nbt", template$entityinfo.entityData); }
    
            nBTTagList1.appendTag(nBTTagCompound1);
        }
    
        NBTList nBTTagList2 = new NBTList();
    
        for (lce::BlockState iBlockState: template$basicPalette) {
            nBTTagList2.appendTag(NBTUtil.writeBlockState(new NBTCompound(), iBlockState));
        }
    
        nbt.setTag("palette", nBTTagList2);
        nbt.setTag("blocks", nBTTagList);
        nbt.setTag("entities", nBTTagList1);
        nbt.setTag("size", this->writeInts(this->size.getX(), this->size.getY(), this->size.getZ()));
        nbt.setString("author", this->author);
        nbt.setInteger("DataVersion", 1139);
        return nbt;
    }
    */

    static inline bool readPosXYZ(const NBTBase& tag, int& x, int& y, int& z) {
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

    void read(NBTCompound& compound) {
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


        for (const auto & entry : nBTTagList1) {
            std::string name = entry("Name").is<std::string>() ? entry("Name").get<std::string>() : std::string{};
            if (name.empty() && entry.is<NBTCompound>()) {
                if (auto opt = entry.get<NBTCompound>().value<std::string>("Name")) name = *opt;
            }
            auto* block = lce::registry::BlockRegistry::getBlockFromName(name);
            if (block == nullptr) {
                throw std::runtime_error("nbt has block state that does not exist");
            }
            int id = 0;
            template$basicPalette.addMapping(block->getDefaultState(), id);
        }

        const NBTBase& blocksTag = compound("blocks");
        if (!blocksTag || !blocksTag.is<NBTList>()) {
            throw std::runtime_error("blocks missing or not a list");
        }
        const auto& blockList = paletteTag.get<NBTList>();

        for (const auto & b : blockList) {
            if (!b.is<NBTCompound>()) continue;

            auto block = b.get<NBTCompound>();

            auto posTag = compound("pos").get<NBTList>();
            Pos3D blockPos(posTag[0].get<i32>(), posTag[1].get<i32>(), posTag[2].get<i32>());

            lce::BlockState iBlockState = template$basicPalette.stateFor(block.getOr("state", -1));
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

private:
    /*
    NBTList writeInts(int... values) {
        NBTList nBTTagList = new NBTList();
    
        for (int i: values) { nBTTagList.appendTag(new NBTTagInt(i)); }
    
        return nBTTagList;
    }
    
    NBTList writeDoubles(double... values) {
        NBTList nBTTagList = new NBTList();
    
        for (double d0: values) { nBTTagList.appendTag(new NBTDouble(d0)); }
    
        return nBTTagList;
    }
    */

    class BasicPalette {
    public:
        static inline const lce::BlockState DEFAULT_BLOCK_STATE = lce::BlocksInit::AIR.getDefaultState();

    private:
        struct BlockStateHash {
            size_t operator()(const lce::BlockState& state) const noexcept {
                return std::hash<int>{}(state.getID() + 16 * state.getDataTag());
            }
        };

        struct BlockStateEqual {
            bool operator()(const lce::BlockState& a, const lce::BlockState& b) const noexcept {
                return a == b;
            }
        };

        std::unordered_map<lce::BlockState, int, BlockStateHash, BlockStateEqual> m_stateToId;
        std::unordered_map<int, lce::BlockState> m_idToState;
        int m_lastId = 0;

    public:
        BasicPalette() = default;

        int idFor(const lce::BlockState& state) {
            auto it = m_stateToId.find(state);

            if (it == m_stateToId.end()) {
                const int newId = m_lastId++;
                m_stateToId.emplace(state, newId);
                m_idToState.emplace(newId, state);
                return newId;
            }

            return it->second;
        }

        ND lce::BlockState stateFor(const int id) const {
            const auto it = m_idToState.find(id);
            return (it == m_idToState.end()) ? DEFAULT_BLOCK_STATE : it->second;
        }

        auto begin() { return m_stateToId.begin(); }
        auto end() { return m_stateToId.end(); }
        ND auto begin() const { return m_stateToId.begin(); }
        ND auto end() const { return m_stateToId.end(); }

        void addMapping(const lce::BlockState& state, int id) {
            m_stateToId.emplace(state, id);
            m_idToState.emplace(id, state);

            if (id >= m_lastId) {
                m_lastId = id + 1;
            }
        }

        ND size_t size() const { return m_stateToId.size(); }

        ND bool contains(const lce::BlockState& state) const {
            return m_stateToId.contains(state);
        }

        ND bool containsId(const int id) const {
            return m_idToState.contains(id);
        }
    };
    
    

};
